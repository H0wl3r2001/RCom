# Passos 

## 1. Disconectar a switch do netlab e conectar o tux a este

1. Ligar o cabo para o porta S0 do computador 3 à switch (T3 - CISCO)
2. Restaurar a configuração do switch(CISCO) - comandos que estão na secretária (quando pedir algum input dar enter)
3. Ligar o cabo para o porta S0 do computador 3 ao router (T3 - CISCO)
4. Restaurar a configuração do router(CISCO) - comandos que estão na secretária (quando pedir algum input dar enter)

5. Para configurar o tux3
6. Configurar o tux3 para ter um endereço ip e ter a rede 172.16.40.0/24 definida
6.1 Executamos os seguintes comandos no terminal do computador 3

```sh
ifconfig eth0 up
ifconfig eth0 172.16.40.1/24
ifconfig eth0
```

7 Configurar o tux4 para ter um endereço ip e ter a rede 172.16.Y0.0/24 definida
7.1 Executamos os seguintes comandos no terminal do computador 4

```sh
ifconfig eth0 up
ifconfig eth0 172.16.40.254/24
ifconfig eth0
```

task4
8. Mac Address IP 
3 : 00:21:51:61:2f:d4 172.16.41.1/24
4 : 00:21:5a:5a:7b:ea 172.16.41.254/24

task5
-TUX3
list current routes

resultado do comando *route -n*

Destination Gateaway Genmask        Flags   Metric Ref Use Iface
172.16.40.0 0.0.0.0  255.255.255.0    u         0   0   0   eth0

resultado do comando *arp -a*
172.16.40.254 dev eth0 lladdr 00:21:5a:5a:7b:ea STALE

-TUX4

list current routes

resultado do comando *route -n*

Destination Gateaway Genmask        Flags   Metric Ref Use Iface
172.16.40.0 0.0.0.0  255.255.255.0    u         0   0   0   eth0

resultado do comando *arp -a*
172.16.40.1 dev eth0 lladdr 00:21:51:61:2f:d4 STALE

task 6

arp -d 172.16.40.254
arp -a

