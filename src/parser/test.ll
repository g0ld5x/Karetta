@format = private constant [14 x i8] c"Hello, world!\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
    %result = call i32 (ptr, ...) @printf(
        ptr @format
    )

    ret i32 0
}