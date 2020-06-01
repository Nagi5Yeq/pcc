program sha1;

type
    sha1_ctx = record
    h:array [0..4] of integer;
    bytes:int64;
    buf:array [0..63] of char
    end;

extern memcpy(dst:^char; src:^char; len:integer):^char;

extern memset(buf:^char; c:char; len:integer):^char;

function sha1_init(ctx:^sha1_ctx):void;
begin
    ctx^.h[0] := 0x67452301;
    ctx^.h[1] := -0x10325477;
    ctx^.h[2] := -0x67452302;
    ctx^.h[3] := 0x10325476;
    ctx^.h[4] := -0x3c2d1e10;
    ctx^.bytes := 0;
end;

function rol(bits:integer; x:integer):integer;
begin
    rol := x shl bits or (x shr (32-bits) and not (-1 shl bits))
end;

function sha1_body(ctx:^sha1_ctx; data:^char; len:integer):^char;
var
    f, k, temp, i, j:integer;
    a, b, c, d, e:integer;
    aa, bb, cc, dd, ee:integer;
    p:^char;
    w:array [0..79] of integer;
begin
    p := data;
    a := ctx^.h[0];
    b := ctx^.h[1];
    c := ctx^.h[2];
    d := ctx^.h[3];
    e := ctx^.h[4];
    repeat begin
        aa := a;
        bb := b;
        cc := c;
        dd := d;
        ee := e;
        for i := 0 to 15 do begin
            w[i] := 0;
            for j := 0 to 3 do
                w[i] := (w[i] shl 8) or (p[i*4+j] and 0xff)
        end;
        for i := 16 to 79 do
            w[i] := rol(1, w[i-3] xor w[i-8] xor w[i-14] xor w[i-16]);
        for i := 0 to 79 do begin
            if i<20 then begin
                f := (b and c) or ((not b) and d);
                k := 0x5a827999
            end
            else if i<40 then begin
                f := b xor c xor d;
                k := 0x6ed9eba1
            end
            else if i<60 then begin
                f := (b and c) or (b and d) or(c and d);
                k := -0x70e44324
            end
            else begin
                f := b xor c xor d;
                k := -0x359d3e2a
            end;
            temp := rol(5, a) + f + e + k + w[i];
            e := d;
            d := c;
            c := rol(30, b);
            b := a;
            a := temp
        end;
        a := a+aa;
        b := b+bb;
        c := c+cc;
        d := d+dd;
        e := e+ee;
        p := p+64;
        len := len-64
    end until len=0;
    ctx^.h[0] := a;
    ctx^.h[1] := b;
    ctx^.h[2] := c;
    ctx^.h[3] := d;
    ctx^.h[4] := e;
    sha1_body := p;
end;

function sha1_update(ctx:^sha1_ctx; data:^char; len:integer):void;
var
    used, free:integer;
begin
    used := ctx^.bytes and 0x3f;
    ctx^.bytes := ctx^.bytes+len;
    if (used<>0) and (used+len<64) then
        memcpy(@ctx^.buf[used], data, len)
    else begin
        if used<>0 then begin
            free := 64-used;
            memcpy(@ctx^.buf[used], data, free);
            data := data+free;
            len := len-free;
            sha1_body(ctx, @ctx^.buf, 64)
        end;
        if len>=64 then begin
            data := sha1_body(ctx, data, len and not 0x3f);
            len := len and 0x3f
        end;
        memcpy(@ctx^.buf[0], data, len)
     end
end;

function sha1_final(md:^char; ctx:^sha1_ctx):void;
var
    used, free, i:integer;
    bits:int64;
begin
    used := ctx^.bytes and 0x3f;
    ctx^.buf[used] := 0x80;
    used := used+1;
    free := 64-used;
    if free<8 then begin
        memset(@ctx^.buf[used], 0, free);
        sha1_body(ctx, @ctx^.buf, 64);
        used := 0;
        free := 64
    end;
    memset(@ctx^.buf[used], 0, free-8);
    bits := ctx^.bytes shl 3;
    for i := 0 to 7 do
        ctx^.buf[i+56] := bits shr (8*(7-i));
    sha1_body(ctx, @ctx^.buf, 64);
    for i := 0 to 19 do
        md[i] := ctx^.h[i div 4] shr (8*(3-i mod 4));
end;

