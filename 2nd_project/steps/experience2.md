1. Ligar o cabo E0 do computador 3 à vlan na porta 1, por exemplo, e  E0 do computador 4 vlan na porta 2.
2. Ligar o cabo E0 do computador 2 à vlan na porta 13

MAC ADDRESS

TUX2 : 00:1f:29:d7:45:c4
IP : 172.16.41.1/24

Criar uma vlan

Switch# configure terminal
Switch(config)# vlan 40
Switch(config)# end
Switch# show vlan id 40

Adicionar uma porta à vlan 

porta 1 à vlan 40

Switch# configure terminal
Enter configuration commands, one per line. End with CNTL/Z.
Switch(config)# interface fastethernet 0/1 (porque ligámos à porta 1 no CISCO)
Switch(config-if)# switchport mode access
Switch(config-if)# switchport access vlan 40
Switch(config-if)# end
Switch# show running-config interface fastethernet 0/1
Switch# show interfaces fastethernet 0/1 switchport

porta 2 à vlan 40

Switch# configure terminal
Enter configuration commands, one per line. End with CNTL/Z.
Switch(config)# interface fastethernet 0/1 (porque ligámos à porta 2 no CISCO)
Switch(config-if)# switchport mode access
Switch(config-if)# switchport access vlan 40
Switch(config-if)# end
Switch# show running-config interface fastethernet 0/2
Switch# show interfaces fastethernet 0/2 switchport

porta 13 à vlan 41

Switch# configure terminal
Enter configuration commands, one per line. End with CNTL/Z.
Switch(config)# interface fastethernet 0/13 (porque ligámos à porta 13 no CISCO)
Switch(config-if)# switchport mode access
Switch(config-if)# switchport access vlan 41
Switch(config-if)# end
Switch# show running-config interface fastethernet 0/2
Switch# show interfaces fastethernet 0/2 switchport


exp2-tux3a -> ping no computador 3 para 4 
exp2-tux3b -> ping no computador 3 para 2 - não deu nada 

exp2-tux2_step8 -> logs do passo 8 do tux 2
exp2-tux3_step8 -> logs do passo 8 do tux 3
exp2-tux2_step8 -> logs do passo 8 do tux 4

exp2-tux2_step10 -> logs do passo 10 do tux 2
exp2-tux3_step10 -> logs do passo 10 do tux 3
exp2-tux4_step10 -> logs do passo 10 do tux 4
