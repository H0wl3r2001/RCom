## SetUp Fisico


### 1. Ligar o cabo para o porta S0 do computador 3 à switch (T3 - CISCO)
### 2. Restaurar a configuração do switch(CISCO) - comandos que estão na secretária (quando pedir algum input dar enter)
### 3. Ligar o cabo para o porta S0 do computador 3 ao router (T3 - CISCO)
### 4. Restaurar a configuração do router(CISCO) - comandos que estão na secretária (quando pedir algum input dar enter)

### 5. Para configurar os *"tuxs"*, voltar a ligar à switch

TUX3 - 172.16.40.1
TUX4 - 172.16.40.254
TUX2 - 172.16.41.1

com este comando em cada TUX
```sh
ifconfig eth0 up
ifconfig eth0 ip/mask
ifconfig eth0
```

### 6. Criar as vlan

#### 1. Ligar o cabo E0 do computador 3 à vlan na porta 2, por exemplo, e  E0 do computador 4 vlan na porta 3.
#### 2. Ligar o cabo E0 do computador 2 à vlan na porta 1

Vlan 40 - TUX3 e TUX4
Vlan 41 - TUX2

com estes comandos num só computador

```sh
Switch# configure terminal
Enter configuration commands, one per line. End with CNTL/Z.
Switch(config)# interface fastethernet 0/port 
Switch(config-if)# switchport mode access
Switch(config-if)# switchport access vlan X
Switch(config-if)# end
Switch# show running-config interface fastethernet 0/1
Switch# show interfaces fastethernet 0/1 switchport
```

### 7. Configurar a eth1
#### 1. Ligar o cabo E1 do computador 4 à vlan 41 na porta 12.

#### 2. Configurar o endereço ip na ethernet 1
Executar este comando no computador 4
```sh
ifconfig eth1 up
ifconfig eth1 172.16.41.253/24
ifconfig eth1
```

NO TUX4
#### 3. Enable IP forwarding.

echo 1 > /proc/sys/net/ipv4/ip_forward

#### 4. Disable ICMP echo ignore broadcast.

 echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts



#### 5. Configurar as rotas

In tuxy3: ip route add 172.16.41.0/24 via 172.16.40.254 

In tuxy2: ip route add 172.16.40.0/24 via 172.16.41.253 

### Verify that the GE1 interface of the Cisco Router is connected to the Lab Router and that GE0 interface of the Cisco Router is connected to the Switch
- Ligar o GE0 ao Switch
- Ligar o GE1 ao 4.1

### Configure the Switch's port from 2 to be on the correct VLAN (41)

addicionar a porta que está ligada ao switch À vlan 41

**hostname gnu-rtr4(ver lab)**

the IP addresses for the interfaces are as indicated in the figure above.

interface FasthEthernet(lab) 0/0
ip address 172.16.41.254 255.255.255.0
no shutdown
ip nat inside

interface FasthEthernet(lab) 0/1
ip address 172.16.2.49 255.255.255.0
no shutdown
ip nat outside

the route to the network 172.16.Y0.0/24 goes through tuxy4 (172.16.Y1.253).

ip route 172.16.40.0 255.255.255.0 172.16.41.253

The default gateway (to 0.0.0.0) is as in the figure (through 172.16.W.254).

ip route 0.0.0.0 0.0.0.0 172.16.2.254 (lab)


#### Modify the configuration file, for NAT so that:
The nat pool ovrld uses the IP address 172.16.W.Y9.

ip nat pool ovrld 172.16.2.49 172.16.2.49 prefix-length 24
ip nat inside source list 1 pool ovrld overload

Verify that the interface for this IP address is configured as outside.
Verify that the interface for the 172.16.Y1.254 IP address is configured as inside.

The access-list 1 permit has the networks 172.16.50.0/24 and 172.16.Y1.0/24 (in the access list command the mask is 0.0.0.7, which is already correct on the file)

#### Configuring the Router

Access the console for configuring the router through the serial port.
Check to see if a tux is already connected to the Router's serial port. If not connect the serial port of the router to one of the tuxes serial port (see the NetLab Network if needed). 
Enter config mode (see below)
Copy-paste the file to the console.
Do show running-config to check the running configuration
Copy the running-config to the startup-config.


extensão
#!/bin/bash
ifcon

configurar a porta 172.16.41.253 à vlan 41
