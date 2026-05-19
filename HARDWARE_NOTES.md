# Celeste PICO-8 → ESP32-S3 移植笔记

## 硬件连接

### LCD (ST7735R 1.44" 128×128, SPI)

| 信号 | ESP32 GPIO | 说明 |
|------|-----------|------|
| SCL  | 14 | SPI 时钟 |
| MOSI | 13 | SPI 数据 |
| RES  | 12 | 复位 |
| DC   | 11 | 数据/命令选择 |
| CS   | 10 | 片选 |
| BLK  | 9  | 背光 (高电平亮) |

SPI 配置：`SPI2_HOST`, Mode 3, 80MHz, DMA 传输。

### 按键 (GPIO 上拉输入, 低电平有效)

| 功能 | GPIO | PICO-8 映射 |
|------|------|------------|
| 左   | 1  | `k_left` |
| 上   | 2  | `k_up` |
| 右   | 3  | `k_right` |
| 下   | 4  | `k_down` |
| 跳   | 5  | `k_jump` |
| 冲   | 6  | `k_dash` |

### LED

GPIO 48, 低电平点亮。

---

## LCD 初始化

**必须使用 `send_init_command2()`**（对应商家的 ST7735R 初始化序列），不能用 `send_init_command()`。

关键寄存器：
- **MADCTL (0x36)**: `0xC8` — MX=1, MY=1, RGB=1 (此面板实际为 RGB 模式)
- **COLMOD (0x3A)**: `0x55` — 16-bit/pixel
- **背光**: `LCD_BLK_Set()` 必须在 init 中调用

---

## 颜色格式

### 最终正确方案

**标准 RGB565**：`(R>>3)<<11 | (G>>2)<<5 | (B>>3)`

此面板用商家 init (MADCTL=0xC8) 后，实际解读为标准 RGB565 顺序。

### DMA 字节序

ESP32 是小端序，DMA 发 `uint16_t` 时低字节先发。但 ST7735 期望高字节先发（与商家 `LCD_WriteData_16Bit` 一致）。

**解决方案**：`display_all()` 发 DMA 前对整个 framebuffer 做 `__builtin_bswap16()`，发完后换回。

```c
void display_all() {
    uint16_t *mem = (uint16_t *)display_mem;
    LCD_Address_Set(0, 0, 127, 127);
    for (int i = 0; i < 128*128; i++) mem[i] = __builtin_bswap16(mem[i]);
    // ... SPI DMA ...
    for (int i = 0; i < 128*128; i++) mem[i] = __builtin_bswap16(mem[i]);
}
```

### PICO-8 → RGB565 查找表

| # | PICO-8 | RGB565 |
|---|--------|--------|
| 0 | #000000 | 0x0000 |
| 1 | #1D2B53 | 0x194A |
| 2 | #7E2553 | 0x792A |
| 3 | #008751 | 0x042A |
| 4 | #AB5236 | 0xAA86 |
| 5 | #5F574F | 0x5AA9 |
| 6 | #C2C3C7 | 0xC618 |
| 7 | #FFF1E8 | 0xFF9D |
| 8 | #FF004D | 0xF809 |
| 9 | #FFA300 | 0xFD00 |
| 10| #FFEC27 | 0xFF64 |
| 11| #00E436 | 0x0726 |
| 12| #29ADFF | 0x2D7F |
| 13| #83769C | 0x83B3 |
| 14| #FF77A8 | 0xFBB5 |
| 15| #FFCCAA | 0xFE75 |

---

## 精灵 Flag 表

来自 `ccleste/tilemap.h` (picotool 从原版 .p8 提取)。

| Flag 位 | 含义 | 涉及 tile |
|---------|------|----------|
| bit 0 (1) | 固体 (solid) | 32-39, 48-55, 66-69, 72, 82-85, 98-101, 112-115 |
| bit 1 (2) | 背景层 | 17, 27, 32-39, 43-44, 48-55, 59-63, 66-69, 70-71, 72, 73-76, 78, 82-85, 86-87, 89-95, 98-101, 105-111, 112-115, 118-127 |
| bit 2 (4) | 冰面装饰 | 16, 40-42, 56-58, 88, 103-104 |
| bit 4 (16) | 冰面 (ice) | 66-69, 82-85, 98-101, 112-115 |

完整数组见 `main/celeste/celeste.c` 中的 `sprite_flags[128]`。

**注意**：尖刺 tile (17, 27, 43, 59) **没有** bit 0 (不是固体)。碰撞由 `spikes_at()` 单独检测，不阻挡移动只造成伤害。

---

## 目录结构

```
main/
├── main.c              # 入口
├── CMakeLists.txt
├── celeste/
│   ├── celeste.c       # 游戏主逻辑 (PICO-8 API 实现)
│   ├── celeste.h
│   ├── sprite.h        # 精灵表 (PICO-8 色号)
│   └── map_data.h      # 地图数据 (4×8 房间, 8192 字节)
└── Drivers/
    ├── LCD/
    │   ├── lcd.c        # 绘图函数 + display_all/display_picture
    │   ├── lcd.h
    │   ├── lcd_init.c   # SPI 初始化 + LCD 初始化
    │   ├── lcd_init.h
    │   ├── lcdfont.h
    │   └── pic.h        # screenshot.png 转换数据
    ├── button/          # 按键驱动
    ├── LED/             # LED 驱动
    └── delay/           # 延时函数
python/
├── pic.py              # PNG → RGB565 C 头文件转换
├── convert_screenshot.py # PNG → 4-bit 索引转换
├── conv_color.py       # 颜色转换工具
├── convert_sprite.py   # 精灵转换工具
└── screenshot.png      # 参考截图
ccleste/                # 参考 Windows 实现 (ccleste)
```

---

## Python 工具

### pic.py — 截图转换

```bash
python python/pic.py
```

将 `python/screenshot.png` 转换为标准 RGB565 并写入 `main/Drivers/LCD/pic.h`。

### 显示图片测试

```c
// main.c 最小图片显示
extern uint16_t rgb565_data[128][128];
void app_main() {
    LCD_Init();
    display_picture(); // 拷贝到 display_mem 然后 display_all()
    while(1) { delay_ms(1000); }
}
```

---

## 游戏逻辑已修复的 Bug

1. **P8fget 永远返回 true** → 添加 `sprite_flags[128]` 表并查表
2. **按键未初始化** → main.c 添加 `button_gpio_init()` 调用
3. **P8spr 忽略 flipx/flipy** → 实现翻转
4. **P8print 空函数** → 添加 4×5 位图字体
5. **P8pal 颜色替换无效** → 用 `pal_map[16]` 数组并在转换循环应用
6. **P8camera 空函数** → 屏幕震动偏移 + shake 衰减
7. **RGB565 公式/字节序/init 序列** → 经过逐位测试+商家代码对照最终确定

---

## 编译烧录

环境：ESP-IDF v6.0.1 @ `C:\esp\v6.0.1\esp-idf`，工具链 @ `C:\Espressif\tools`

```batch
@echo off
set IDF_PATH=C:\esp\v6.0.1\esp-idf
set IDF_TOOLS_PATH=C:\Espressif\tools
set IDF_PYTHON_ENV_PATH=C:\Espressif\tools\python\v6.0.1\venv
REM ... PATH 设置 ...
idf.py -p COM8 build flash
```
