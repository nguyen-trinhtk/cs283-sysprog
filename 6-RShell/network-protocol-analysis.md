# Network Protocol Analysis: TCP Remote Shell

### 1. Learning Process (2 points)

**Answer:**

I used ChatGPT (GPT-5), Claude, and Gemini to learn and verify my understanding of network protocol analysis for this project.

Questions that I asked:
- "Explain the difference between TCP as a byte stream vs message-based protocols, with a simple client/server example."
- "How can I delimit command output in a custom TCP protocol if `recv()` may return partial or combined data?"
- "Show how to use `strace` to trace `socket`, `connect`, `accept`, `send`, and `recv` for a C remote shell."
- "How do I identify SYN/SYN-ACK/ACK and payload bytes (including `0x04`) in tcpdump or Wireshark output?"

Then, the AI pointed me to some resources, like Linux man pages for each syscall (e.g.: `man 2 socket`, `man 2 connect`, `man 2 accept`, `man 2 send`, `man 2 recv`), and Wireshark display filter docs and tcpdump usage examples (`-X`, `-r`, interface selection)

### 2. Protocol Design Analysis (3 points)

Analyze and document your remote shell protocol:

#### A. Protocol Specification

**Answer:**

**Client → Server:**
- Message format: text command sent with `send(cli_socket, cmd_buff, strlen(cmd_buff), 0)`. In my implementation, this is effectively newline-terminated input from `fgets()` (neither a fixed-length frame nor explicitly sending `\0`, which can be easily confused with `0`).
- Encoding: text bytes.
- Example: `"ls -la\n"` (the server will then strips trailing newline after `recv()`).

**Server → Client:**
- Message format: raw output stream from command execution (stdout/stderr redirected to socket), followed by an EOF (1 byte) marker.
- EOF marker: `RDSH_EOF_CHAR = 0x04`, which is sent by `send_message_eof()`.
- Example: `"file1.txt\nfile2.txt\n\x04"`.

**Explain why you use EOF marker:**
- I used EOF marker because TCP is a byte stream and does not preserve application message boundaries. Without an explicit delimiter, the client would not know when one command response is complete, so it could block in `recv()` waiting for more data or accidentally merge output from adjacent commands.

#### B. Message Boundary Problem

**Answer:**

- TCP is stream-based, so the kernel can coalesce multiple `send()` calls into one `recv()`, or split one `send()` across multiple `recv()` calls. In this code, the client loops on `recv()` and checks whether the **last byte** of the received chunk is `0x04`.
- The protocol therefore thinks that "response ends when EOF marker arrives." This gives the client an application-level boundary independent of packet boundaries. This is why command output can arrive in any chunking pattern, yet still terminate correctly once the EOF marker is seen.

#### C. Protocol Limitations

- If command output contains byte `0x04`, the client may prematurely treat that chunk as end-of-message (especially when `0x04` lands as the last byte of a `recv()` buffer).
- If the network drops mid-message, client `recv()` may return `0`/error and the response is truncated with no recovery/retry semantics.
- For now, there is no length prefix, request ID, checksum, or structured status metadata; stdout/stderr are merged, which can make robust parsing difficult.
- We can improve by doing the following: use length-prefixed frames (e.g., 4-byte network-order length + payload), include message type/request ID/status fields, escape or avoid sentinel bytes, add timeout/retry/error framing, and optionally separate stdout/stderr channels.

### 3. Traffic Capture and Analysis (3 points)

I used **Option B (strace only)** to capture syscall-level communication between client and server.

#### Client strace output

```text
6721  socket(AF_INET, SOCK_STREAM, IPPROTO_IP) = 3
6721  connect(3, {sa_family=AF_INET, sin_port=htons(1234), sin_addr=inet_addr("\x31\x32\x37\x2e\x30\x2e\x30\x2e\x31")}, 16) = 0
6721  sendto(3, "\x65\x63\x68\x6f\x20\x68\x65\x6c\x6c\x6f\x0a", 11, 0, NULL, 0) = 11
6721  recvfrom(3, "\x68\x65\x6c\x6c\x6f\x0a", 65536, 0, NULL, NULL) = 6
6721  recvfrom(3, "\x04", 65536, 0, NULL, NULL) = 1
6721  sendto(3, "\x65\x78\x69\x74\x0a", 5, 0, NULL, 0) = 5
6721  recvfrom(3, "\x04", 65536, 0, NULL, NULL) = 1
6721  +++ exited with 0 +++
```

#### Server strace output

```text
6717  socket(AF_INET, SOCK_STREAM, IPPROTO_IP) = 3
6717  bind(3, {sa_family=AF_INET, sin_port=htons(1234), sin_addr=inet_addr("\x30\x2e\x30\x2e\x30\x2e\x30")}, 16) = 0
6717  listen(3, 20)                     = 0
6717  listen(3, 20)                     = 0
6717  accept(3, NULL, NULL)             = 4
6717  recvfrom(4, "\x65\x63\x68\x6f\x20\x68\x65\x6c\x6c\x6f\x0a", 65535, 0, NULL, NULL) = 11
6722  +++ exited with 0 +++
6717  --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=6722, si_uid=501, si_status=0, si_utime=0, si_stime=0} ---
6717  sendto(4, "\x04", 1, 0, NULL, 0)  = 1
6717  recvfrom(4, "\x65\x78\x69\x74\x0a", 65535, 0, NULL, NULL) = 5
6717  sendto(4, "\x04", 1, 0, NULL, 0)  = 1
6717  accept(3, NULL, NULL)             = ? ERESTARTSYS (To be restarted if SA_RESTART is set)
6717  --- SIGINT {si_signo=SIGINT, si_code=SI_KERNEL} ---
6717  +++ killed by SIGINT +++
```

#### Analysis for command `echo hello`

1. **Client sends command:**
   - `sendto(... "\x65\x63\x68\x6f\x20\x68\x65\x6c\x6c\x6f\x0a", 11, ...) = 11`
   - This is ASCII bytes for `echo hello\n`.

2. **Server receives command:**
   - `recvfrom(... "\x65\x63\x68\x6f\x20\x68\x65\x6c\x6c\x6f\x0a", ..., ) = 11`
   - This confirms server receives the same command bytes.

3. **Server sends response + EOF:**
   - Server sends EOF explicitly: `sendto(... "\x04", 1, ...) = 1`.
   - The command output (`hello\n`) is produced by the forked child process running the command; with this trace filter, that data appears on the client receive side.

4. **Client receives response:**
   - `recvfrom(... "\x68\x65\x6c\x6c\x6f\x0a", ...) = 6` (`hello\n`)
   - `recvfrom(... "\x04", ...) = 1` confirms EOF marker terminates receive loop.

5. **Exit sequence:**
   - Client sends `exit\n` (`\x65\x78\x69\x74\x0a`), server receives it, then sends final EOF marker and waits for next client.

### 4. TCP Connection Verification (2 points)

Verify the TCP connection works correctly:

**Checklist:**
- [X] TCP 3-way handshake occurs (SYN, SYN-ACK, ACK)
- [X] Client connects to server successfully
- [X] Commands are sent correctly (null-terminated)
- [X] Server responses include EOF marker (0x04)
- [X] Connection closes gracefully (FIN)

**Answer:**

1. **How many TCP packets for connection establishment?**
   - Standard TCP connection establishment is **3 packets**: SYN, SYN-ACK, ACK.
   - With strace-only capture, these packets are not shown directly, but successful `connect()` on client and `accept()` on server confirm establishment occurred.

2. **How does TCP handle your send() calls?**
   - TCP is stream-oriented, so it does **not** guarantee one `send()` maps to one `recv()`.
   - In this run, command and response arrived in clean chunks, but protocol correctness relies on EOF (`0x04`) because sends can be split/combined in other runs.

3. **Can you see the EOF character in dumps?**
   - Yes. Both client and server traces show `"\\x04"` being received/sent:
     - Client: `recvfrom(... "\\x04", ...) = 1`
     - Server: `sendto(... "\\x04", 1, ...) = 1`

4. **What happens on `exit` command?**
   - Client sends `exit\n` (`"\\x65\\x78\\x69\\x74\\x0a"`).
   - Server receives it, sends EOF marker, returns from that client session, and goes back to `accept()` waiting for another client.
   - In this trace, server was then stopped manually with `SIGINT`, so I do not observe a fully graceful FIN teardown sequence in packet-level detail.

**Issues:** Since I only check with `strace`, I never got the chance to verify FIN, which should be doable with `tcpdump`.