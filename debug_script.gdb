set pagination off
print sizeof($1)/sizeof(int)
set $i = 0
set $current = $1[$i]
while ($i < $2)
	set $current = $1[$i]
	break $current
	set $i = $i + 1
end
set $i = 0
run
while (1)
	disassemble
	info registers rax
	info registers rbx
	info registers rcx
	info registers rdx
	info registers rip
	info registers rbp
	info registers rsp
	info registers r8
	info registers r9
	info registers r10
	info registers r11
	continue
end
quit
