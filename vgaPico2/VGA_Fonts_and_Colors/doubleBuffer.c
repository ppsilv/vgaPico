/*
Exatamente! Você captou perfeitamente o conceito. 

## Sim, é exatamente isso! 

O sistema de DMA com dois canais em cadeia foi projetado justamente para facilitar o **double buffering** e operações como **scroll**.

## Como funciona na prática:

### 1. **Estrutura de Dados:**
*/
// Dois buffers para double buffering
uint8_t vga_buffer_0[SCREEN_WIDTH * SCREEN_HEIGHT];
uint8_t vga_buffer_1[SCREEN_WIDTH * SCREEN_HEIGHT];

// Ponteiro que será atualizado pelo DMA
uint32_t address_pointer = (uint32_t)vga_buffer_0;
 

//### 2. **Alternando Buffers:**

// Para fazer scroll ou alternar buffers:
void swap_buffers() {
    if (address_pointer == (uint32_t)vga_buffer_0) {
        address_pointer = (uint32_t)vga_buffer_1;
    } else {
        address_pointer = (uint32_t)vga_buffer_0;
    }
    
    // Aqui você pode preencher o buffer que não está sendo exibido
    fill_back_buffer();
}

void scroll_screen(int pixels) {
    // Alterna o buffer
    swap_buffers();
    
    // Copia o conteúdo com offset para criar o efeito de scroll
    uint8_t* current_buffer = (address_pointer == (uint32_t)vga_buffer_0) ? vga_buffer_1 : vga_buffer_0;
    uint8_t* back_buffer = (address_pointer == (uint32_t)vga_buffer_0) ? vga_buffer_0 : vga_buffer_1;
    
    // Copia com offset (exemplo: scroll de 1 linha)
    memcpy(back_buffer, &current_buffer[SCREEN_WIDTH * pixels], 
           (SCREEN_HEIGHT - pixels) * SCREEN_WIDTH);
    
    // Preenche a nova área que apareceu com o scroll
    memset(&back_buffer[(SCREEN_HEIGHT - pixels) * SCREEN_WIDTH], 0, 
           pixels * SCREEN_WIDTH);
}
/*

## Vantagens deste sistema:

### **⏰ Timing Perfeito:**
- O DMA canal 1 atualiza o ponteiro **exatamente quando** o canal 0 termina de transferir o frame atual
- **Sem tearing**: A troca acontece durante o vertical blanking

### **🚀 Zero Overhead de CPU:**
- A troca de buffers é automática via DMA
- CPU fica livre para: 
  - Processar entradas
  - Calcular física do jogo
  - Gerar novos conteúdos

### **🎮 Scroll Suave:**
```c
// Exemplo: scroll horizontal suave
void smooth_horizontal_scroll() {
    static int scroll_offset = 0;
    
    scroll_offset++;
    if (scroll_offset >= SCREEN_WIDTH) scroll_offset = 0;
    
    // Atualiza o ponteiro para começar a exibir a partir do offset
    address_pointer = (uint32_t)vga_buffer_0 + scroll_offset;
}
```

## Fluxo Completo:
1. **DMA 0** → Transfere frame atual do endereço apontado por `address_pointer`
2. **Ao terminar** → Automaticamente inicia DMA 1
3. **DMA 1** → Atualiza `dma_hw->ch[rgb_chan_0].read_addr` com novo valor de `address_pointer`
4. **DMA 1 termina** → Automaticamente reinicia DMA 0
5. **CPU** → Enquanto isso, prepara o próximo frame no buffer não visível

É uma arquitetura muito elegante para gráficos em tempo real! 🎯
*/

/*
Excelente pergunta! O timing é crucial para evitar **tearing** (rasura na tela). Você precisa trocar os buffers no momento exato.

## 🕒 O Momento Correto: Durante o VBlank (Vertical Blanking)

O momento seguro para trocar buffers é durante o **período de blanking vertical**, quando o feixe de elétrons do CRT (ou o equivalente em LCD) está retornando ao topo da tela.

## Como detectar o VBlank:

### 1. **Usando IRQ do DMA (Recomendado):**
*/
// Configurar IRQ no canal 1 (que sinaliza quando um frame terminou)
void setup_dma_irq() {
    dma_channel_set_irq0_enabled(rgb_chan_1, true);  // IRQ quando canal 1 completar
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

// Handler de IRQ
void dma_irq_handler() {
    dma_hw->ints0 = (1u << rgb_chan_1);  // Clear IRQ
    buffer_swap_pending = true;          // Sinaliza que pode trocar buffer
}

volatile bool buffer_swap_pending = false;
uint8_t* front_buffer = vga_buffer_0;
uint8_t* back_buffer = vga_buffer_1;

void main() {
    setup_dma_irq();
    
    while (true) {
        // 1. Renderiza no back buffer
        render_frame(back_buffer);
        
        // 2. Espera pelo sinal de VBlank
        while (!buffer_swap_pending) {
            // Pode fazer outras tarefas aqui
            __wfe();  // Wait For Event (economiza energia)
        }
        
        // 3. Troca os buffers de forma segura
        swap_buffers();
        buffer_swap_pending = false;
    }
}

void swap_buffers() {
    // Troca os ponteiros
    uint8_t* temp = front_buffer;
    front_buffer = back_buffer;
    back_buffer = temp;
    
    // Atualiza o address_pointer para o próximo frame
    address_pointer = (uint32_t)front_buffer;
}



void wait_for_vblank() {
    // Espera até que o DMA esteja transferindo as últimas linhas
    while (dma_channel_is_busy(rgb_chan_0)) {
        // Espera ativa - não ideal, mas funcional
    }
    
    // Pequena margem de segurança
    busy_wait_us(100);
}

// Uso:
void main() {
    while (true) {
        render_frame(back_buffer);
        wait_for_vblank();
        swap_buffers();
    }
}

/*
## 📊 **Fluxo Seguro:**

1. **Renderize** → No back buffer (buffer não visível)
2. **Espere** → Pelo sinal de VBlank (IRQ ou polling)
3. **Troque** → `address_pointer` para apontar para o novo buffer
4. **Repita** → O DMA automaticamente pegará o novo buffer no próximo frame

## 🎯 **Dica Pro:**
Se estiver usando PIO para VGA, você pode usar o próprio sinal de VSync do VGA para detectar o blanking vertical com ainda mais precisão!

A chave é **sincronizar a troca com o retorno vertical** do feixe! 🚀

*/