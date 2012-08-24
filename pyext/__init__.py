
import tvnet as c_tvnet
import sys

c_tvnet.tvu_init(sys.argv[0])

encode = c_tvnet.encode
decode = c_tvnet.decode
cuid = c_tvnet.cuid
socket = c_tvnet.socket
bind = c_tvnet.bind
unbind = c_tvnet.unbind
client_bind = c_tvnet.client_bind
sendto = c_tvnet.sendto
recvfrom = c_tvnet.recvfrom
close = c_tvnet.close

