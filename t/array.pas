program arr;

type
    a1=integer;
    a2=array[0..5] of a1;
    a3=array[0..6] of a2;
    a4=array[0..7] of a3;
    a5=array[0..8] of a4;

extern memset(dst:^void; n:integer; c:integer):^void;
extern puts(s:string):integer;

function assert_equal(a:integer; b:integer):void;
begin
    if a<>b then puts('error!')
end;

function main():integer;
var
    a:array[0..9] of a5;
    pa1:^a5;
    pa2:^a4;
    pa3:^a3;
    pa4:^a2;
    pa5:^a1;
begin
    memset(@a, 0, 4*6*7*8*9*10);
    a[1][2][3][4][5] := 0x1234567;
    pa1 := @a[1];
    pa2 := @a[1][2];
    pa3 := @a[1][2][3];
    pa4 := @a[1][2][3][4];
    pa5 := @a[1][2][3][4][5];
    assert_equal(pa1^[2][3][4][5], 0x1234567);
    assert_equal(pa2^[3][4][5], 0x1234567);
    assert_equal(pa3^[4][5], 0x1234567);
    assert_equal(pa4^[5], 0x1234567);
    assert_equal(pa5^, 0x1234567);
    
    assert_equal((@a[1][2][3])^[4][5], 0x1234567);
    assert_equal((@a[1][2][2]+1)^[4][5], 0x1234567);

    puts('good');
    main := 0
end;
