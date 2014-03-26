moveR offset 48
ref this
set this as addr
output this

when this
  decr
done

assign this to ptr

ref this
set this as addr
incr by 49

decr by 58 when this
  incr by 58 output this
  moveR
  set this as left
  incr
  decr by 58
done
