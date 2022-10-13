%define NEXT_KEY 0
%macro colon 2

%2:
dq NEXT_KEY
db %1, 0

%define NEXT_KEY %2
%endmacro
