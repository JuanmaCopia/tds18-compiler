hola=$(cat -n assembly.s | grep --invert-match '.*:\|.*\..*' | cut -f1)
hola=$(echo $hola | tr ' ' ',')
echo $hola
gdb -batch executable --init-eval-command="print {$hola}" -x debug_script.gdb 2> /dev/null | grep --invert-match '\[Inferior .*'
