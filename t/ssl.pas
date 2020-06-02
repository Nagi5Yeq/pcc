program ssl;

type
    hostent=record
        h_name:^char;
        h_aliases:^^char;
        h_addrtype:integer;
        h_length:integer;
        h_addr_list:^^char
    end;
    sockaddr_in=record
        sin_family:short;
        sin_port:short;
        sin_addr:array[0..3] of char;
        sin_zero:array[0..7] of char
    end;

const
    AF_INET=2;
    SOCK_STREAM=1;
    IPPROTO_TCP=6;

extern memset(dst:^void; n:integer; size:int64):^void;
extern memcpy(dst:^void; src:^void; size:int64):^void;
extern sprintf(s:^char; fmt:^char; ...):integer;
extern printf(fmt:^char; ...):int64;

extern socket(domain:integer; socktype:integer; protocol:integer):integer;
extern htons(port:short):short;
extern gethostbyname(name:^char):^hostent;
extern connect(s:integer; addr:^sockaddr_in; size:integer):integer;

extern TLS_method():^void;
extern SSL_CTX_new(meth:^void):^void;
extern SSL_new(meth:^void):^void;
extern SSL_set_fd(ssl:^void; fd:integer):integer;
extern SSL_connect(ssl:^void):integer;
extern SSL_write(ssl:^void; buf:^void; num:integer):integer;
extern SSL_read(ssl:^void; buf:^void; num:integer):integer;
extern SSL_shutdown(ssl:^void):integer;
extern SSL_free(ssl:^void):void;
extern SSL_CTX_free(ssl:^void):void;

function main(argc:integer; argv:^^char):integer;
var
    s, len, size, thissize:integer;
    ssl, ctx:^void;
    host:^hostent;
    addr:sockaddr_in;
    buf:array[0..1023] of char;
    p:^char;
begin
    s := socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(@addr, 0, 16);
    addr.sin_family := AF_INET;
    addr.sin_port := htons(443);
    host := gethostbyname(argv[1]);
    p := host^.h_addr_list[0];
    memcpy(@addr.sin_addr, p, 4);
    connect(s, @addr, 16);
    ctx := SSL_CTX_new(TLS_method());
    ssl := SSL_new(ctx);
    SSL_set_fd(ssl, s);
    SSL_connect(ssl);
    len := sprintf(@buf, 'GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n', argv[1]);
    size := 0;
    while len>0 do begin
        thissize := SSL_write(ssl, @buf[size], len - size);
        len := len - thissize;
        size := size + thissize;
    end;
    while true do begin
        size := SSL_read(ssl, @buf, 1023);
        if size<=0 then break;
        buf[size] := '\0';
        printf('%s', @buf);
    end;
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    main := 0;
end;

