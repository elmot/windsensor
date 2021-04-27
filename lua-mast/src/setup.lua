cfg={
ssid="ELM_NAVI1",
pwd="YAYAYANUS",
auth=wifi.WPA2_PSK,
channel=3,
hidden=false,
max=4,
beacon=100,
save=false
}

wifi.ap.config(cfg)
wifi.setmode(wifi.SOFTAP,false)
