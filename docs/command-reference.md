# GCU command reference

All command names below are provided in `commands.txt`. Names formerly written with punctuation have been normalized: `dir?` → `dir`, `alodir?` → `alodir`, `no!` → `no`, and `yes!` → `yes`.

**Status key:** **Native** means the command has a C implementation in this tree. **Host mapping** means GCU invokes a corresponding program from the host `PATH`; that host utility must be installed. GCU is not yet a fully self-contained native replacement for all 110 commands.

| Command | Purpose / invocation | Status |
|---|---|---|
| `cowf` | `cowf [FILE...]` — concatenate file contents; stdin when no file is given. | Native |
| `fcow` | `fcow [OPTIONS] [FILE...]` — reverse records. | Host mapping: `tac` |
| `numl` | `numl [OPTIONS] [FILE...]` — number lines. | Host mapping: `nl` |
| `wfof` | `wfof [OPTIONS] [FILE...]` — display bytes in selected bases. | Host mapping: `od` |
| `b32` | `b32 [OPTIONS] [FILE]` — Base32 encode/decode. | Host mapping: `base32` |
| `b64` | `b64 [OPTIONS] [FILE]` — Base64 encode/decode. | Host mapping: `base64` |
| `bnc` | `bnc [OPTIONS] [FILE]` — base encoding helper. | Host mapping: `base64` |
| `rff` | `rff [OPTIONS] [FILE...]` — reflow text. | Host mapping: `fmt` |
| `pag` | `pag [OPTIONS] [FILE...]` — paginate text. | Host mapping: `pr` |
| `warp` | `warp [OPTIONS] [FILE...]` — fold long lines. | Host mapping: `fold` |
| `ftop` | `ftop [-COUNT] [FILE]` — first lines. | Native |
| `fend` | `fend [-COUNT] [FILE]` — last lines. | Native |
| `fspl` | `fspl [OPTIONS] FILE [PREFIX]` — split a file. | Host mapping: `split` |
| `cspat` | `cspat [OPTIONS] FILE REGEX` — split at matching lines. | Host mapping: `csplit` |
| `wcnt` | `wcnt [-lwmc] [FILE...]` — count lines, words, bytes, and characters. | Native |
| `bsum` | `bsum [FILE...]` — legacy checksum. | Host mapping: `sum` |
| `cksm` | `cksm [FILE...]` — POSIX CRC checksum. | Host mapping: `cksum` |
| `md5c` | `md5c [FILE...]` — MD5 digest (legacy). | Host mapping: `md5sum` |
| `b2ck` | `b2ck [FILE...]` — BLAKE2 digest. | Host mapping: `b2sum` |
| `sh1c` | `sh1c [FILE...]` — SHA-1 digest (legacy). | Host mapping: `sha1sum` |
| `sh224` | `sh224 [FILE...]` — SHA-224 digest. | Host mapping: `sha224sum` |
| `sh256` | `sh256 [FILE...]` — SHA-256 digest. | Host mapping: `sha256sum` |
| `sh384` | `sh384 [FILE...]` — SHA-384 digest. | Host mapping: `sha384sum` |
| `sh512` | `sh512 [FILE...]` — SHA-512 digest. | Host mapping: `sha512sum` |
| `ordf` | `ordf [OPTIONS] [FILE...]` — sort records. | Host mapping: `sort` |
| `shufx` | `shufx [OPTIONS] [FILE]` — shuffle lines. | Host mapping: `shuf` |
| `dupf` | `dupf [OPTIONS] [INPUT [OUTPUT]]` — filter adjacent duplicate lines. | Host mapping: `uniq` |
| `lcom` | `lcom FILE1 FILE2` — compare sorted files. | Host mapping: `comm` |
| `dtop` | `dtop [FILE]` — topologically order dependency pairs. | Host mapping: `tsort` |
| `ptix` | `ptix [FILE...]` — keyword-in-context index. | Host mapping: `ptx` |
| `fldx` | `fldx [OPTIONS] [FILE...]` — select fields or byte positions. | Host mapping: `cut` |
| `mrgc` | `mrgc [FILE...]` — merge lines into columns. | Host mapping: `paste` |
| `jkey` | `jkey [OPTIONS] FILE1 FILE2` — join records. | Host mapping: `join` |
| `chrx` | `chrx [OPTIONS] SET1 [SET2]` — translate/delete characters. | Host mapping: `tr` |
| `t2sp` | `t2sp [OPTIONS] [FILE...]` — expand tabs. | Host mapping: `expand` |
| `s2tb` | `s2tb [OPTIONS] [FILE...]` — convert spaces to tabs. | Host mapping: `unexpand` |
| `see` | `see [-a] [-l] [PATH...]` — list directory entries. | Native |
| `dir` | `dir [PATH...]` — brief directory listing. | Native |
| `alodir` | `alodir [PATH...]` — detailed directory listing. | Native |
| `dclr` | `dclr` — report directory color configuration. | Native |
| `cop` | `cop SOURCE DEST` — copy one file. | Native |
| `dblk` | `dblk OPERAND=VALUE...` — block copy and conversion. | Host mapping: `dd` |
| `instme` | `instme [OPTIONS] SOURCE... DEST` — copy/install files. | Host mapping: `install` |
| `movef` | `movef SOURCE DEST` — move or rename a path. | Native |
| `rom` | `rom FILE...` — remove directory entries (not recursive). | Native |
| `wipf` | `wipf [OPTIONS] FILE...` — overwrite/remove files; not guaranteed secure erasure. | Host mapping: `shred` |
| `hlnk` | `hlnk EXISTING NEWLINK` — create hard link. | Native |
| `lnkf` | `lnkf TARGET LINK` — create symbolic link. | Native |
| `mdir` | `mdir [-p] [-m MODE] DIRECTORY...` — create directories. | Native |
| `mkq` | `mkq NAME...` — create named pipes. | Native |
| `nodef` | `nodef NAME TYPE [MAJOR MINOR]` — create a special node. | Host mapping: `mknod` |
| `rlnk` | `rlnk SYMLINK...` — print link targets. | Native |
| `cpath` | `cpath PATH...` — resolve canonical paths. | Native |
| `rdem` | `rdem DIRECTORY...` — remove empty directories. | Native |
| `ulnk` | `ulnk FILE` — unlink one path. | Native |
| `ownf` | `ownf OWNER[:GROUP] FILE...` — change ownership. | Host mapping: `chown` |
| `grpown` | `grpown GROUP FILE...` — change group ownership. | Host mapping: `chgrp` |
| `permf` | `permf MODE FILE...` — change permissions. | Host mapping: `chmod` |
| `punch` | `punch FILE...` — create files if absent. | Native |
| `dfree` | `dfree [OPTIONS] [PATH...]` — report filesystem space. | Host mapping: `df` |
| `dusage` | `dusage [OPTIONS] [PATH...]` — estimate disk usage. | Host mapping: `du` |
| `fstat` | `fstat FILE...` — show file metadata. | Native |
| `fsync` | `fsync [FILE...]` — flush files or system buffers. | Native |
| `truncf` | `truncf [OPTIONS] FILE...` — resize files. | Host mapping: `truncate` |
| `type` | `type [-n] [TEXT...]` — print text (echo-like). | Native |
| `fprn` | `fprn FORMAT [ARG...]` — formatted output. | Host mapping: `printf` |
| `no` | `no [STRING...]` — repeatedly print a response. | Native |
| `true` | `true` — return success. | Native |
| `false` | `false` — return failure. | Native |
| `testmp` | `testmp EXPRESSION` — evaluate a test expression. | Host mapping: `test` |
| `exval` | `exval EXPRESSION` — evaluate an expression. | Host mapping: `expr` |
| `mout` | `mout [OPTIONS] [FILE...]` — copy stdin to stdout and files. | Host mapping: `tee` |
| `mbn` | `mbn NAME [SUFFIX]` — remove path prefixes/suffix. | Host mapping: `basename` |
| `mdn` | `mdn NAME` — print directory portion of a path. | Host mapping: `dirname` |
| `pchk` | `pchk PATH...` — basic pathname validation. | Native |
| `jroot` | `jroot ROOT COMMAND [ARG...]` — change root and run a command. | Host mapping: `chroot` |
| `envx` | `envx [NAME=VALUE...] [COMMAND...]` — set environment/run command. | Native |
| `prio` | `prio [OPTIONS] COMMAND...` — adjust process niceness. | Host mapping: `nice` |
| `stay` | `stay COMMAND...` — run while ignoring hangup. | Host mapping: `nohup` |
| `wai` | `wai` — print current working directory. | Native |
| `ttycfg` | `ttycfg [SETTING...]` — inspect/change terminal settings. | Host mapping: `stty` |
| `bufctl` | `bufctl MODE COMMAND...` — control stdio buffering. | Host mapping: `stdbuf` |
| `idme` | `idme [OPTIONS] [USER]` — show user/group identity. | Host mapping: `id` |
| `grps` | `grps [USER]` — show group membership. | Host mapping: `groups` |
| `wimlog` | `wimlog` — show logged-in users. | Host mapping: `who` |
| `usrlite` | `usrlite` — show login name. | Host mapping: `logname` |
| `usrs` | `usrs` — show users on the system. | Host mapping: `users` |
| `huh` | `huh` — print real user ID. | Native |
| `wami` | `wami` — print effective user name. | Native |
| `wimarch` | `wimarch` — print machine architecture. | Native |
| `wit` | `wit [OPTIONS]` — date/time display. | Host mapping: `date` |
| `hidn` | `hidn` — print host ID. | Host mapping: `hostid` |
| `wimh` | `wimh` — print hostname. | Host mapping: `hostname` |
| `ncore` | `ncore` — print available processor count. | Host mapping: `nproc` |
| `mos` | `mos` — print system information. | Native |
| `upsys` | `upsys` — show uptime. | Host mapping: `uptime` |
| `wimt` | `wimt` — print terminal name. | Host mapping: `tty` |
| `ctxset` | `ctxset [OPTIONS] FILE...` — change security context. | Host mapping: `chcon` |
| `ctxrun` | `ctxrun [OPTIONS] COMMAND...` — run with a security context. | Host mapping: `runcon` |
| `tmo` | `tmo DURATION COMMAND...` — run with a time limit. | Host mapping: `timeout` |
| `sigsend` | `sigsend SIGNAL PID...` — send a process signal. | Host mapping: `kill` |
| `mmsleep` | `mmsleep DURATION` — pause for a duration. | Host mapping: `sleep` |
| `factr` | `factr NUMBER...` — print prime factors. | Host mapping: `factor` |
| `ftgn` | `ftgn [OPTIONS] NUMBER...` — format numbers with units. | Host mapping: `numfmt` |
| `seqn` | `seqn [OPTIONS] LAST` — print a numeric sequence. | Host mapping: `seq` |
| `tmpf` | `tmpf [OPTIONS] [TEMPLATE]` — create a temporary file/directory. | Host mapping: `mktemp` |
| `envshow` | `envshow [VARIABLE...]` — print environment values. | Native |
| `pmad` | `pmad` — shell integration helper; must be sourced separately. | Native notice |
| `dumpme` | `dumpme [OPTIONS] [FILE...]` — display file data in hex. | Host mapping: `hexdump` |
| `yes` | `yes [STRING...]` — repeatedly print a response. | Native |

## Important compatibility notes

- Host mappings preserve argument boundaries and call `execvp`; they do not invoke a shell.
- The current mapping does not translate every historical Gill-specific flag to the host program's flags. Check the host utility's help before relying on less common options.
- The command `type` may be shadowed by a shell builtin of the same name. Use `./build/bin/type` or `command -p`/an explicit path to run the GCU executable when needed.
- `pmad` cannot update the parent shell environment when executed as a child process; source its shell integration file instead.
