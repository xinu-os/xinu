Xipx
====

For now, I'm just putting information here that I think is important and
is at risk of being lost. Someday, this should probably be a more
polished and complete page. ...yes, someday, someone will have to make
it that... someone...

User threads global variables problem: User threads cannot have global
variables because global variables apparently don't survive the objcopy
to binary, or something. The discovery: assigning to a global variable
in a usrthr causes a pagefault, because that write tries to go to some
high memory address, but apparently that memory address doesn't get put
into the binary, so the codesize (that gets prepended to the user thread
binary) isn't large enough to cover that address, so when the thread is
created it doesn't map a page for that spot, so pagefault. It's probably
just a matter of changing some build options or something, but I haven't
dug into it yet.

Limits on code locations: The GETPROT\_ADDR in the Makefile defines
where the binary image for getprotected.S/initPaging.c gets put in
memory. This must be low enough so that the whole image stays within a
16-bit address range because it all executes in 16-bit real mode. Also,
keep IMG\_ADDR low enough to not exceed 0x28ffffff (there is no safety
check to make sure that IMG\_ADDR + [xipx image size] < 0x29000000, so
things will just break (probably page fault) if you mess that up.)

Custom memcpy: a memcpy optimized for x86 is used in place of the libxc
implementation. So my libxc memcpy.c has a #ifndef \_SCC\_ in it.

TODO: talk about details of ut\_to\_k\_view() and justify why it does
everything it does in the order it does it. (This might end up in the
thesis.)

TODO: describe the interaction of x86 interrupts and privilege levels,
etc. (Might also be in the thesis.)

Syscalls (and maybe all interrupts?) currently push CR3 before switching
to kernel memory mapping, then pop it when switching back. This seems
dumb -- why not just use the thrtab[thrcurrent].pdloc instead? Then you
don't need to push it, and that hacky addition to the prepmigthr()
function in lutmigd.c is not needed.

Room to improve: Probably not EVERY syscall needs to switch to kernel
view of memory. Figure out which ones don't need to (probably everything
that won't involve using the kernel heap or stack) and make it so they
don't. Make sure this won't break anything else, e.g. will context
switching to another thread while in the memory view of the current
thread be a problem?
