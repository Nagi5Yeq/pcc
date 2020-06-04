program qsort;

extern srand(seed:integer):void;
extern rand():integer;
extern time(t:integer):integer;
extern printf(fmt:string; ...):integer;

function qsort(arr:^integer; start:integer; ends:integer):void;
var
    i,j,mid,t:integer;
begin
    if ends>start then begin
        mid := arr[ends];
        i := start;
        for j := start to ends-1 do
            if arr[j]<mid then begin
                t := arr[i];
                arr[i] := arr[j];
                arr[j] := t;
                i := i+1;
            end;
        t := arr[i];
        arr[i] := arr[ends];
        arr[ends] := t; 
        qsort(arr, start, i-1);
        qsort(arr, i+1 ,ends)
    end
end;

function main():integer;
var
    arr:array[0..99999] of integer;
    i:integer;
begin
    srand(time(0));
    for i:=0 to 99999 do
        arr[i] := rand() mod 1000;
    qsort(@arr, 0, 99999);
    for i:=99999 downto 0 do
        printf('%d ', arr[i]);
    main := 0
end;
