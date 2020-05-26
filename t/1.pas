program a;

function test(a:integer;b:char;c:real;):integer;
var
    d:integer;
    e:char;
    f:string;
begin
    a:=1;
    b:='a';
    c:=3.5;
    a:=b*(c-b+6)--4;
    d:=a*a-b-(5+c);
    e:='\x03';
    f:='a normal test \'\\\0\023';
    f[4];
    test:=e+d;
end;
