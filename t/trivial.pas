program a;

function arithmetic_test(a:integer;b:char;c:real):integer;
var
    d:integer;
    e:char;
    f:real;
begin
    a:=1;   d:=-2;
    b:='B'; e:='\x46';
    c:=1.5; f:=-3.14;

    a:=a+b-c+d;
    b:=b*c-d*e;
    c:=c+d*e+f;

    b:=1; e:=2;
    a:=1/2+b/e;
    d:=1 div 2 + b div e;

    a:=a and e or b xor d;
    d:=(a shl 3) xor (not a shr b);
    arithmetic_test:=a+d
end;

function string_test():char;
var 
    s:string;
    a,b:char;
begin
    s:='hello world';
    a:=s[3];
    b:=s[6];
    string_test:=s[b-a]
end;

function pointer_test():integer;
var
    a,b:integer;
    p:^integer;
    s:string;
    ps:^char;
begin
    a:=1;
    p:=@a;
    b:=p[0];
    s:='testabcd';
    ps:=s+2;
    pointer_test:=b+ps[3];
end;

function recursion_test(x:integer):integer;
begin
    if (x=0) or (x=1) then
        recursion_test:=1
    else
        recursion_test:=recursion_test(x-1)+recursion_test(x-2)
end;

function branch_test(a:integer):integer;
var
    b,c:integer;
begin
    b:=3;
    c:=0;
    if a<b then
        c:=-1
    else
        if a>b then
            c:=1;
    while b>0 do
    begin
        a:=a*b;
        b:=b-1
    end;
    repeat
    begin
        a:=a*c;
        b:=b+1
    end
    until b>5;
    branch_test:=a;
end;
