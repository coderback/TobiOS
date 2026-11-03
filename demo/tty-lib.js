// Emscripten JS library: connects the program's file descriptors to a browser terminal.
// The C++ is compiled unmodified. std::getline(std::cin, ...) ends up in fd_read, which
// suspends the wasm stack with ASYNCIFY until the page has a line of input for it.
addToLibrary({
  fd_write: (fd, iov, iovcnt, pnum) => {
    let total = 0;
    for (let i = 0; i < iovcnt; i++) {
      const ptr = HEAPU32[(iov + i * 8) >> 2];
      const len = HEAPU32[(iov + i * 8 + 4) >> 2];
      if (len) Module.ttyWrite(fd, HEAPU8.slice(ptr, ptr + len));
      total += len;
    }
    HEAPU32[pnum >> 2] = total;
    return 0;
  },

  fd_read__async: true,
  fd_read: (fd, iov, iovcnt, pnum) => Asyncify.handleAsync(async () => {
    if (fd !== 0) return 8; // EBADF: stdin is the only descriptor this program reads
    const bytes = await Module.ttyRead();
    let done = 0;
    for (let i = 0; i < iovcnt && done < bytes.length; i++) {
      const ptr = HEAPU32[(iov + i * 8) >> 2];
      const len = HEAPU32[(iov + i * 8 + 4) >> 2];
      const n = Math.min(len, bytes.length - done);
      HEAPU8.set(bytes.subarray(done, done + n), ptr);
      done += n;
    }
    if (done < bytes.length) Module.ttyUnread(bytes.subarray(done));
    HEAPU32[pnum >> 2] = done;
    return 0;
  }),
});
