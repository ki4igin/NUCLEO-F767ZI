clear

% echoudp("on",2020);

u_tx = udpport("LocalPort", 2022);
u_rx = udpport("LocalPort", 2020, "Timeout", 1);

write(u_tx, 1:16, "single", "192.168.0.10", 2020);  
data = read(u_rx, 16, "single");

test_vector = 1:2048;

% figure;
    
for i = 1:1000
    write(u_tx, test_vector, "single", "192.168.0.10", 2020);  
    data = read(u_rx, length(test_vector), "single");
    plot(data);
    pause(1);
end