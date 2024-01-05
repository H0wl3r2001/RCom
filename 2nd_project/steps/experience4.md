porta 16 à vlan 41

Switch# configure terminal
Enter configuration commands, one per line. End with CNTL/Z.
Switch(config)# interface fastethernet 1/16 (porque ligámos à porta 1 no CISCO)
Switch(config-if)# switchport mode access
Switch(config-if)# switchport access vlan 41
Switch(config-if)# end
Switch# show running-config interface fastethernet 1/16
Switch# show interfaces fastethernet 0/1 switchport



## Step 4
Enable IP forwarding.

# echo 1 > /proc/sys/net/ipv4/ip_forward

Disable ICMP echo ignore broadcast.

# echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

## Step 5

ETH0
MAC_ADDRESS         IP_ADDRESS       
00:21:5a:5a:7b:ea  172.16.40.254

ETH1
MAC_ADDRESS         IP_ADDRESS       
00:c0:df:25:1a:f4  172.16.41.253

## Step 6

ip route add 172.16.41.0/24 via 172.16.40.254 

ip route add 172.16.Y0.0/24 via 172.16.Y1.253 

## Step 6

#Cisco Router

Verify that the GE0 interface of the Cisco Router is connected to the Lab Router.  == Ligar o GE0 ao 4.1
Verify that the GE1 interface of the Cisco Router is connected to the Switch. == Ligar o GE1 ao Router



