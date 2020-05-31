program hello_world;

extern puts(s:string):integer;
extern printf(s:string, ...):integer;
extern __isoc99_scanf(s:string, ...):integer;

function main():integer;
var
    buf:array[0..1023] of char;
begin
    puts('Hello, World!');
    printf('Variadic Function Test: %d, %s, %x\nsay something:', 1, 'yes', 0x12345678);
    __isoc99_scanf('%s', @buf);
    printf('you said: %s\n', @buf)
end;
