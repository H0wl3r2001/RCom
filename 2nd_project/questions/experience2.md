# 1.How to configure vlan40?

Using this commands:

- To create a vlan 40
```sh
# configure terminal
(config)# vlan 40
(config)# end
# show vlan id 40
```

- To add a port to vlan 40

```sh
# configure terminal
(config)# interface fastethernet 0/1
(config-if)# switchport mode access
(config-if)# switchport access vlan Y0
(config-if)# end
# show running-config interface fastethernet 0/1
# show interfaces fastethernet 0/1 switchport
```

According to the script, we added the ports corresponding to the computers ip address 172.16.40.1 and 172.16.40.254 to vlan 40.

# 2.How many broadcast domains are there? How can you conclude it from the logs?

Analysing the logs:

- When doing ping -b 172.16.40.255 in TUX3:

We got only the response of TUX4 because TUX2 is not in the same vlan.

- When doing ping -b 172.16.41.255 in TUX2:

We didn't have any response, thus TUX2 is the only port in vlan 41.

In conclusion there are two broadcast domains : 172.16.40.255 and 172.16.41.255

