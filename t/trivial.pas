program a;
const
    gb=true;
    gc='a';
    gi=5;
    gr=3.1415;
    gs='nagi';
var
    gv:integer;

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

function array_test():integer;
var
    ai:array['a'..'z'] of integer;
    ac:array[0..25] of char;
    i:integer;
    c:char;
begin
    i:=0;
    c:='z';
    repeat
    begin
        ac[i]:=c;
        ai[c]:=i;
        i:=i+1;
        c:=c-1
    end until i>25;
    array_test:=ai['f']-ac[5];
end;

function pointer_test():integer;
var
    a,b:integer;
    c:array[0..10] of integer;
    p,q:^integer;
    s:string;
    ps:^char;
begin
    a:=1;
    p:=@c[0];
    q:=@c[4];
    b:=q-p;
    p[0]:=a;
    q[b]:=a;
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
    b,c,d:integer;
begin
    b:=3;
    c:=0;
    d:=4;
    if a<b then
        c:=-1
    else
        if a>b then
            c:=1;
    while b>0 do
    begin
        while d>0 do
        begin
            d:=d-1;
            if d=3 then
            begin
                d:=2;
                break
            end;
            a:=a*d+b;
        end;
        b:=b-1
    end;
    repeat
    begin
        repeat
        begin
            d:=d+1;
            if d=3 then
            begin
                d:=4;
                continue;
            end;
            a:=a*c+d;
        end
        until d>5;
        a:=a*c;
        b:=b+1
    end
    until b>5;
    branch_test:=a;
end;

function name_test_global():integer;
begin
    gv:=1;
    name_test_global:=gv+gb+gc+gi+gr+gs[3];
end;

function name_test_local():integer;
const
    gb=false;
    gc='A';
    gi=10;
    gr=2.71828;
    gs='Hanjie Wu';
var
    gv:integer;
begin
    gv:=0;
    name_test_local:=gv+gb+gc+gi+gr+gs[3];
end;

function name_test():boolean;
begin
    name_test:=name_test_global()<>name_test_local()
end;

function void_test():void;
var
    a,b:^void;
begin
end;
