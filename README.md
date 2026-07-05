# Commando → Mega Drive, versão SGDK (mesma Opção C, casca diferente)

## Status honesto

- **`src/z80_interp.68k`** — o interpretador Z80, IDÊNTICO ao da versão raw-asm,
  só com um ajuste (`Z80_UNIMPL` agora retorna com `z80_trapped=1` em vez de
  travar o 68000 num loop infinito, pra você poder mostrar isso na tela via C).
  **Continua montando limpo** com binutils genérico — testei aqui de novo.
- **`src/commando_io.c` / `commando_rom.c` / `main.c`** — reescritos em C
  usando API do SGDK (`JOY_readJoypad`, `SYS_doVBlankProcess`). Sintaxe
  checada com gcc contra um `genesis.h` fake (pega erro de digitação, não
  garante que a API bate 100% com a versão real do SGDK que você tem
  instalada — não tenho o SGDK real neste sandbox pra compilar de verdade).
- **`src/io_shim.68k`** — a peça nova e a única com risco real: adapta a
  convenção de registrador que o interpretador usa (`d0`/`d1`) pra uma
  chamada C normal (por pilha). **Esse é o primeiro lugar a olhar se o link
  falhar** — se o compilador do SGDK passar argumento por registrador em vez
  de pilha, é só ajustar este arquivo, o C não precisa mudar.
- **Sumiu de propósito**: `header.68k`, `startup.68k`, `megadrive.x`,
  `Int_VBlank` manual — o SGDK já faz tudo isso (TMSS, init de VDP, vetores,
  joypad). Não foi esquecido, foi substituído.

## Como montar (no seu ambiente Windows com SGDK já instalado)

1. Copie `inc/` e `src/` pra dentro de um projeto SGDK novo (estrutura padrão:
   pasta com `src/`, `res/`, e o `Makefile` copiado do
   `SGDK/skel/makefile.gen` como você já faz nos outros projetos).
2. `make` normal do seu setup.
3. Se `io_shim.68k` der erro de link (símbolo duplicado ou argumento errado
   sendo lido do lado C), é a convenção de chamada — me manda a mensagem
   exata de erro.

## O que falta (igual à versão raw-asm, não mudou)

Mesma lista de sempre: colar sua dump em `commando_rom.c`, DD/FD não
implementado (trapeia limpo agora, mostra mensagem na tela em vez de
travar), conversão de gráficos não começada, driver Z80 de som não escrito,
mapeamento de input/DSW é placeholder.
