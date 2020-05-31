program hello_world;

extern puts(s:string):integer;
extern printf(s:string, ...):integer;
extern __isoc99_scanf(s:string, ...):integer;

function main():integer;
var
    buf:array[0..1023] of char;
    i:integer;
begin
    puts('Hello, World!');
    printf('Variadic Function Test: %d, %s, %x\n', 1, 'yes', 0x12345678);
    for i:=0 to 10 do
    begin
        if i=3 then continue;
        if i=5 then break;
        printf('say something: ');
        __isoc99_scanf('%s', @buf[0]);
        printf('round %d you said: %s\n', i, @buf[0])
    end;
end;
