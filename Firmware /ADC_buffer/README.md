# Medição dos canais de ADC com Timer de 1ms 


## Estrutura do código
- [main](main) - Pasta aonde está contido dos headers e códigos necessários 
- [main.c](main/main.c) - Código necessário para juntar todos os códigos 
- [libraries.h](main/libraries.h) - Arquivo onde está contido todas as bibliotecas utilizadas
- [global.c](main/global.c) & [global.h](main/global.h) - Arquivos responsáveis pelas declarações das variáveis globais
- [Setup_adc.c](main/Setup_adc.c) & [Setup_adc.h](main/Setup_adc.h) - Arquivos onde está as configurações do ADC
- [Tasks.c](main/Tasks.c) & [Tasks.h](main/Tasks.h) - Arquivos onde está contido as tasks que serão usadas
- [Timer.c](main/Timer.c) & [Timer.h](main/Timer.h) - Arquivos de configuração do Timer 
---
## main.c

O [main.c](main/main.c) é o ponto de entrada do firmware para o ESP32. Ele inicializa tarefas e configura o sistema operacional FreeRTOS.

### Estrutura do Código `main.c`
```c
#include "libraries.h"
#include "global.h"

TaskHandle_t Handle_voltage_read;

void app_main(void)
{
    xTaskCreate(voltage_read, "voltage_read", 2048, NULL, tskIDLE_PRIORITY + 1, &Handle_voltage_read);
}
```

### Explicação
- **Inclusão de bibliotecas**: `libraries.h` para dependências gerais e `global.h` para variáveis globais.
- **Criação de Task**: `xTaskCreate(voltage_read, "voltage_read", 2048, NULL, tskIDLE_PRIORITY + 1, &Handle_voltage_read);`
  - `voltage_read`: Função da tarefa.
  - `"voltage_read"`: Nome da tarefa.
  - `2048`: Tamanho da pilha em bytes.
  - `NULL`: Nenhum parâmetro passado.
  - `tskIDLE_PRIORITY + 1`: Prioridade da tarefa.
  - `&Handle_voltage_read`: Referência para controle da tarefa.

---

## libraries.h

O [libraries.c](main/libraries.h) centraliza todas as bibliotecas e arquivos de cabeçalho do projeto, facilitando a organização e manutenção do código.

### Estrutura do Arquivo `libraries.h`
```c
#ifndef libraries_H
#define libraries_H

#include <stdio.h>
#include "esp_timer.h"
#include "Timer.h"
#include "Setup_adc.h"
#include "Tasks.h"
#include "global.h"
#include <driver/adc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"

#endif
```

### Explicação
- **Proteção contra inclusão múltipla**: Usa `#ifndef` e `#define`.
- **Bibliotecas padrão**: `stdio.h` para entrada e saída.
- **ESP-IDF**: `esp_timer.h`, `adc.h`, `esp_log.h` para controle de tempo, ADC e logs.
- **FreeRTOS**: `FreeRTOS.h`, `task.h`, `ringbuf.h` para multitarefa e buffers.
- **Arquivos do projeto**: `Timer.h`, `Setup_adc.h`, `Tasks.h`, `global.h` para configuração e gerenciamento do sistema.

---

## Global.c e Global.h

Os arquivos [global.c](main/global.c) e [global.h](main/global.h) são usados para definir e gerenciar uma variável global que armazena as medições do ADC (Conversor Analógico-Digital). O uso de um cabeçalho (`.h`) e um arquivo de implementação (`.c`) permite uma organização modular do código, facilitando a reutilização e a manutenção do sistema.

### Estrutura do Arquivo `global.h`

```c
#ifndef GLOBAL_H
#define GLOBAL_H

// Definindo a struct de medições do ADC
typedef struct {
    float tensao_medida;
    float corrente_medida;
} Medicoes_ADC;

// Declaração externa da variável global
extern Medicoes_ADC medidas;

#endif // GLOBAL_H
```

### Explicação dos Elementos do `global.h`

1. **Definição da `struct` `Medicoes_ADC`**
   - A estrutura `Medicoes_ADC` é usada para armazenar duas medições:
     - `tensao_medida`: representa a tensão lida pelo ADC.
     - `corrente_medida`: representa a corrente lida pelo ADC.

2. **Uso de `extern` para a variável `medidas`**
   - `extern Medicoes_ADC medidas;` declara a variável global `medidas` sem definir a memória para ela.
   - Isso permite que a variável seja compartilhada entre diferentes arquivos de código sem causar erros de redefinição.



## Estrutura do Arquivo `global.c`

```c
#include "global.h"

// Definindo a variável global para armazenar as medições do ADC
Medicoes_ADC medidas;
```

### Explicação dos Elementos do `global.c`
1. **Inclusão do Arquivo de Cabeçalho (`#include "global.h"`)**
   - Isso garante que a estrutura `Medicoes_ADC` e a declaração da variável global `medidas` estejam disponíveis para uso.

2. **Definição da Variável Global `medidas`**
   - Aqui, a variável `medidas` é definida, alocando a memória correspondente.
   - Qualquer arquivo que inclua `global.h` poderá acessar essa variável, garantindo um armazenamento centralizado para os dados do ADC.
  
---

## Setup_ADC.c e Setup_ADC.h

### Explicação do código `setup_adc.c`

O [Setup_adc.c](main/Setup_adc.c) configura e inicializa os canais do ADC no ESP32 para leitura de tensão, corrente e temperatura.

### Estrutura do Código
```c
#include "libraries.h"
#include "global.h"

int adc_value_2;
int adc_value_3;
int adc_value_1;

void adc_setup(void) {
    // Inicializar o ADC
    adc1_channel_t channel2 = ADC1_CHANNEL_2;
    adc1_channel_t channel3 = ADC1_CHANNEL_3;
    adc1_channel_t channel4 = ADC1_CHANNEL_4;
    adc1_config_width(ADC_WIDTH_BIT_12);

    // Configurar os canais ADC 2, 3 e 4, atenuação
    adc1_config_channel_atten(channel2, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(channel3, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(channel4, ADC_ATTEN_DB_0);
}

void adc_init_Reading() {
    adc_value_2 = adc1_get_raw(ADC1_CHANNEL_2);
    adc_value_3 = adc1_get_raw(ADC1_CHANNEL_3);
    medidas.tensao_medida = ((float)adc_value_2 /ADC_Max_value) * V_REF;
    medidas.corrente_medida = ((float)adc_value_3 /ADC_Max_value) * V_REF;

    // Debug: Mostra valor sendo armazenado e posição atual
    printf("Armazenando tensão %.3f V na posição %d do buffer\n", 
           medidas.tensao_medida, medidas.index_primeiro);

    // Armazena a tensão medida no buffer firstLevel
    medidas.PrimeiraCamada[medidas.index_primeiro] = medidas.tensao_medida;

    // Incrementa o índice e faz o wrap-around quando chegar ao final
    medidas.index_primeiro = (medidas.index_primeiro + 1) % PrimeiraCamada_Length;

    if (medidas.index_primeiro >= 191) {
        printf("\n--- Início do processamento do buffer ---\n");
        medidas.index_primeiro = 0;
        
        // Calcula a média do primeiro nível
        float soma = 0.0;
        for(int i = 0; i < PrimeiraCamada_Length; i++) {
            soma += medidas.PrimeiraCamada[i];
            // Debug: Mostra a cada 32 amostras somadas
            if ((i + 1) % 32 == 0) {
                printf("Soma parcial após %d amostras: %.3f\n", i + 1, soma);
            }
        }
        float media = soma / PrimeiraCamada_Length;
        
        // Debug: Mostra detalhes do cálculo
        printf("Soma total: %.3f\n", soma);
        printf("Média calculada: %.3f V\n", media);
        
        // Armazena a média no segundo nível
        medidas.SegundaCamada[medidas.index__segundo] = media;
        
        // Debug: Mostra onde foi armazenado no segundo nível
        printf("Média armazenada na posição %d do segundo nível\n", 
               medidas.index__segundo);
        
        // Incrementa o índice do segundo nível
        medidas.index__segundo = (medidas.index__segundo + 1) % SecondLevel_Length;
        printf("Armazenando tensão %.3f V na posição %d do Segundo nivel\n", 
            media, medidas.index__segundo);
        
        medidas.primeiro_nivel_ciclos++;
        printf("Ciclo %lu completado\n", (unsigned long)medidas.primeiro_nivel_ciclos);
        printf("--- Fim do processamento do buffer ---\n\n");
    }
}

void adc_init_temp(void) {
    // Leitura da temperatura
    adc_value_1 = adc1_get_raw(ADC1_CHANNEL_4);
    float temp_medida = (adc_value_1 / (float)ADC_Max_value) * Max_Value_Voltage; // Ajustar fórmula
    printf("Temperatura Medida: %.2f\n", temp_medida);
}
```

### Explicação
- **`adc_setup()`**: Configura os canais ADC 2, 3 e 4 com resolução de 12 bits e atenuação de 0dB.
- **`adc_init_Reading()`**:







- **`adc_init_temp()`**:
  - Lê o valor do ADC no canal 4 para temperatura.
  - Converte para tensão (necessita ajuste na equação).
  - Exibe o valor medido no console.
 
## Explicação do Arquivo `Setup_adc.h`

O [Setup_adc.h](main/Setup_adc.h) define constantes e declara funções para configuração e leitura do ADC no ESP32.

### Estrutura do Código
```c
#ifndef ADC_CONFIG_H
#define ADC_CONFIG_H

#define ADC_Max_value 4095
#define Max_Value_Voltage 190.7
#define V_REF 1100.0 // Referência do ADC em mV (ajuste conforme necessário)

void adc_setup(void);
void adc_init_Reading();
void adc_init_temp(void);

#endif // ADC_CONFIG_H
```

### Explicação
- **Proteção contra inclusão múltipla**: Garante que o arquivo seja incluído apenas uma vez.
- **Definição de constantes**:
  - `ADC_Max_value`: Valor máximo do ADC (12 bits = 4095).
  - `Max_Value_Voltage`: Valor de tensão máximo utilizado no cálculo.
  - `V_REF`: Referência do ADC (em mV), ajustável conforme necessário.
- **Declaração de funções**:
  - `adc_setup()`: Configuração do ADC.
  - `adc_init_Reading()`: Inicialização da leitura de tensão e corrente.
  - `adc_init_temp()`: Leitura e conversão da temperatura.

---

## Explicação do Arquivo `Tasks.h`

### Introdução
O arquivo [Tasks.h](main/Tasks.h) contém a definição das funções das tarefas utilizadas no FreeRTOS para leitura e armazenamento de dados do ADC no ESP32.

### Estrutura do Código
```c
#ifndef TASKS_H
#define TASKS_H

// Tamanho do buffer em bytes
#define BUFFER_SIZE 768

void writer_task(void *param);
void voltage_read(void *param);

#endif
```

### Explicação
- **Proteção contra inclusão múltipla**: O uso de `#ifndef TASKS_H` garante que o cabeçalho seja incluído apenas uma vez.
- **Definição de `BUFFER_SIZE`**: Define o tamanho do buffer utilizado para armazenar os dados do ADC.
- **Declaração das funções**:
  - `writer_task(void *param)`: Responsável por armazenar dados do ADC e enviá-los para um ringbuffer.
  - `voltage_read(void *param)`: Executa a leitura periódica da tensão e corrente.

## Explicação do Arquivo `Tasks.c`

### Introdução
O arquivo [Tasks.c](main/Tasks.c) implementa as funções declaradas em `Tasks.h`, realizando a leitura do ADC e o armazenamento de dados em um ringbuffer.

### Estrutura do Código
```c
#include "libraries.h"
#include "global.h"

// Ponteiro para o ringbuffer
static RingbufHandle_t ringbuf;

void writer_task(void *param) {
    float adc_data[192];
    while (1) {
        for (int i = 0; i < 192; i++) {
            // adc_data[i] = Leitura do ADC (comentado no código original)
        }

        if (xRingbufferSend(ringbuf, adc_data, sizeof(adc_data), pdMS_TO_TICKS(1000)) != pdTRUE) {
            printf("Falha ao enviar dados para o ringbuffer\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void voltage_read(void *param) {
    printf("Task Voltage Read iniciando...\n");
    iniciar_timer();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Tensão medida: %.2f \n", medidas.tensao_medida);
        printf("Corrente medida: %.2f \n", medidas.corrente_medida);
    }
}
```

### Explicação
- **`writer_task`**:
  - Coleta dados do ADC e os armazena em um array.
  - Envia os dados para um ringbuffer usando `xRingbufferSend()`.
  - Aguarda 1 segundo (`vTaskDelay()`) antes de repetir o ciclo.
- **`voltage_read`**:
  - Inicia um timer (`iniciar_timer()`) para realizar a leitura periódica.
  - A cada segundo, exibe os valores de tensão e corrente armazenados na variável global `medidas`.

 --

## Explicação dos Arquivos `Timer.h` e `Timer.c`

Os arquivos `Timer.h` e `Timer.c` são responsáveis pela configuração e controle de timers no ESP32 utilizando a API `esp_timer`. Eles fornecem a funcionalidade de temporizador periódico, permitindo a execução de tarefas de forma automática em intervalos definidos.


## `Timer.h`

### Estrutura do Código

```c
#ifndef TIMER_H
#define TIMER_H

#include "esp_timer.h"

// Função para inicializar o timer
void iniciar_timer();

// Variável global para monitoramento
extern volatile uint32_t contador_chamadas;

#endif
```

### Explicação

- **Proteção contra Inclusão Múltipla**: O uso de `#ifndef TIMER_H` impede que o cabeçalho seja incluído mais de uma vez.
- **Definição da Função `iniciar_timer`**: A função `iniciar_timer()` será responsável por inicializar o timer no código.
- **Declaração da Variável `contador_chamadas`**: A variável `contador_chamadas` é declarada como `extern` para ser usada globalmente. Ela será usada para contar o número de vezes que o timer é acionado.


## `Timer.c`

### Estrutura do Código

```c
#include "libraries.h"
#include "global.h"

volatile uint32_t contador_chamadas = 0; // Contador global para monitoramento

void Timer_ISR(void *param) {
    adc_init_Reading(); // Atualiza os valores da variável global "medidas"
}

void iniciar_timer(void) {

    const esp_timer_create_args_t my_timer_args = {
        .callback = &Timer_ISR,
        .name = "Meu Timer"
    };

    esp_timer_handle_t timer_handler;
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 2000000)); // 2 segundos
}
```

### Explicação

- **Contador Global `contador_chamadas`**: A variável `contador_chamadas` é usada para monitorar o número de vezes que o timer foi acionado. Ela é incrementada a cada execução do timer.
  
- **Função `Timer_ISR`**: A função de interrupção do timer, chamada `Timer_ISR`, é executada periodicamente. Ela chama a função `adc_init_Reading()`, que realiza a leitura do ADC e atualiza as medições de tensão e corrente na variável global `medidas`.

- **Função `iniciar_timer`**:
  - **Criação do Timer**: A função utiliza `esp_timer_create` para criar um timer periódico.
  - **Início do Timer**: Com a função `esp_timer_start_periodic`, o timer é iniciado com um intervalo de 2 segundos (2.000.000 microsegundos).
  - **Tratamento de Erros**: `ESP_ERROR_CHECK` é usada para verificar erros durante a criação e inicialização do timer.





