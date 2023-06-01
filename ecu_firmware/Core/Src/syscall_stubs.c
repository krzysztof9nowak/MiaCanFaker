/* This should shut linker up */

void __attribute__((weak)) _close(void)
{
}

void __attribute__((weak)) _fstat(void)
{
}

void __attribute__((weak)) _isatty(void)
{
}

void __attribute__((weak)) _lseek(void)
{
}

void __attribute__((weak)) _read(void)
{
}

void __attribute__((weak)) _write(void)
{
}

