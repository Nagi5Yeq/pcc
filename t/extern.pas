program hello_world;

extern Pascal_puts(s:string):integer;
extern Pascal_printf(s:string, ...):integer;
extern Pascal_scanf(s:string, ...):integer;

function main():integer;
var
    buf:array[0..1023] of char;
    i:integer;
begin
    Pascal_puts('Hello, World!');
    Pascal_printf('Variadic Function Test: %d, %s, %x\n', 1, 'yes', 0x12345678);
    for i:=0 to 10 do
    begin
        if i=3 then continue;
        if i=5 then break;
        Pascal_printf('say something: ');
        Pascal_scanf('%s', @buf[0]);
        Pascal_printf('round %d you said: %s\n', i, @buf[0])
    end;
end;
