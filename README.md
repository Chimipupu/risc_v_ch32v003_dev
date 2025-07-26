# RISC-Vマイコン CH32V003評価F/W開発
WCH製 RISC-Vマイコン CH32V003の評価F/W個人開発リポジトリ

## 開発環境

### H/W

- 評価基板
  - `CH32V003EVT(CH32V003F4P6-R0-1V1)`

- マイコン ... 型番:[CH32V003F4P6](https://akizukidenshi.com/catalog/g/g118061/)🔗
  - CPU ... [QingKeV2 (32bit RV32EC RISC-V)](https://wch-ic.com/downloads/QingKeV2_Processor_Manual_PDF.html)🔗
  - ROM ... 16KB
  - RAM ... 2KB
  - Clock ... 48MHz
  - GPIO ... 18本
  - DMA ... x7ch
  - タイマー ... 16bit汎用タイマ x4本
  - I2C ... x1ch
  - SPI ... x1ch
  - UART ... x1ch
  - ADC ... 10bit SAR x8ch

### S/W

- IDE/SDK/コンパイラ
  - [MounRiver Studio (MRS) V2.20](https://www.mounriver.com/download)🔗
- 実装ベース
  - [CH32V003EVT V2.1](https://www.wch.cn/downloads/CH32V003EVT_ZIP.html)🔗

### デバッガ

- [WCH-LinkE Ver1.3](https://akizukidenshi.com/catalog/g/g118065)🔗

### デバッグ

#### SWD

- [WCH-LinkE <--> 評価基板]
  - SWDIO <--> PD1ピン
  - GND <--> GND

#### UART

- [WCH-LinkE <--> 評価基板]
  - RX <--> PD5ピン(TX)
  - TX <--> PD6ピン(RX)
  - GND <--> GND
