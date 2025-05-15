# Relatório de Análise de Tráfego de Rede

## 1. Introdução
Este projeto tem como objetivo analisar o tráfego de rede no Mininet e Wireshark, composta por quatro hosts e um switch, utilizando Python, Matplotlib e outros frameworks para extração e visualização de métricas.

## 2. Metodologia
### 2.1 Ferramentas
- Wireshark 
- Mininet 
- Python
- Frameworks
  - Pandas
  - NumPy
  - Scapy
  - Matplotlib
  - Seaborn

### 2.2 Processo de Captura
- Topologia: 4 hosts e um switch OpenFlow
- Por meio do uso do Wiresharkm, captura dos pacotes ICMP feita no switch com filtro
- Imagens de captura de Wireshark salvas

## 3. Desenvolvimento
### 3.1 Código Python
#### 3.1.1 Imports e instalações
```python
%pip install scapy
%pip install matplotlib
%pip install numpy
%pip install pandas
%pip install seaborn

from scapy.all import rdpcap, IP, ICMP
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime
import pandas as pd
import seaborn as sns
```
#### 3.1.2 Extração de elementos do arquivo pcap
```python
def leitura_pcap(pcap_file):
    # Leitura do arquivo pcap
    Pacotes = rdpcap(pcap_file)
    
    # Listas com informações dos pacotes
    ips_origem = []
    ips_destino = []
    tamanho_pacotes = []
    timestamps = []
    
    # Extração de informações diretamente do arquivo pcap
    for Pacote in Pacotes:
        # Checa se o pacote é um pacote IP e ICMP
        if IP in Pacote and ICMP in Pacote:
            ips_origem.append(Pacote[IP].src)
            ips_destino.append(Pacote[IP].dst)
            timestamps.append(float(Pacote.time))
            tamanho_pacotes.append(len(Pacote))
    
    timestamps = np.array(timestamps)
    tamanho_pacotes = np.array(tamanho_pacotes)
    
    # Calculo das métricas com base nas informações extraídas
    # Métricas temporais
    tempo = timestamps[-1] - timestamps[0]
    intervalos = np.diff(timestamps)
    intervalo_medio = np.mean(intervalos)

    # Métricas de pacotes
    n_pacotes = len(Pacotes)  
    n_bytes = sum(tamanho_pacotes)
    vazao = n_bytes * 8 / tempo

    
    # Organização das informações em um DataFrame Pandas com timestamps, tamanho dos pacotes, ips de origem e destino
    df = pd.DataFrame({
        'timestamp': timestamps,
        'tamanho_pacote': tamanho_pacotes,
        'ip_origem': ips_origem,
        'ip_destino': ips_destino
    })
    
    # Print basic statistics
    print(f"Número de pacotes totais: {n_pacotes}")
    print(f"Número de bytes totais: {n_bytes}")
    print(f"Duração em segundos: {tempo:.2f} seconds")
    print(f"Vazão média: {vazao/1000:.2f} kbps")
    print(f"Intervalo médio entre pacotes: {intervalo_medio*1000:.2f} ms")

    return df
    
```
#### 3.1.3 Plotagem dos graficos
```python
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def plotagem_graficos(df):
    # Extração de dados do DataFrame
    timestamps = df['timestamp']
    tamanho_pacotes = df['tamanho_pacote']
    tempo = timestamps.iloc[-1] - timestamps.iloc[0] if len(timestamps) > 1 else 0
    intervalos = np.diff(timestamps) if len(timestamps) > 1 else []
    n_bytes = np.sum(tamanho_pacotes)

    plt.figure(figsize=(15, 10))

    # Plot 1: Tamanho dos pacotes ao longo do tempo
    plt.subplot(2, 3, 1)
    if len(timestamps) > 1:
        # Plotar tamanho dos pacotes ao longo do tempo
        plt.plot(timestamps - timestamps.iloc[0], tamanho_pacotes, marker='o', linestyle='-')
        plt.title('Tamanho dos Pacotes ao Longo do Tempo', fontsize=14)
        plt.xlabel('Tempo (segundos)', fontsize=12)
        plt.ylabel('Tamanho do Pacote (bytes)', fontsize=12)

    # Plot 2: Histograma dos intervalos entre pacotes
    plt.subplot(2, 3, 2)
    if len(intervalos) > 0:
        # Filtrar outliers (intervalos maiores que o percentil 95, ajuda na visualização do gráfico)
        limite_superior = np.percentile(intervalos, 95)
        intervalos_filtrados = intervalos[intervalos <= limite_superior]

        # Plotar histograma com linha de densidade
        sns.histplot(intervalos_filtrados * 1000, bins=20, kde=True, edgecolor='black', alpha=0.7)
        plt.title('Distribuição dos Intervalos entre Pacotes', fontsize=14)
        plt.xlabel('Intervalo (ms)', fontsize=12)
        plt.ylabel('Frequência', fontsize=12)
        plt.xlim(0, limite_superior * 1000) 

    # Plot 3: Throughput acumulado
    plt.subplot(2, 3, 3)
    if len(timestamps) > 0:
        # Calculo do throughput acumulado
        cumulative_bytes = np.cumsum(df['tamanho_pacote'])
        plt.plot(timestamps - timestamps.iloc[0], cumulative_bytes * 8 / 1000, linestyle='-')
        plt.title('Throughput Acumulado', fontsize=14)
        plt.xlabel('Tempo (s)', fontsize=12)
        plt.ylabel('kbits transmitidos', fontsize=12)

    # Plot 4: Densidade dos Intervalos
    plt.subplot(2, 3, 4)
    if len(intervalos) > 0:
        # Utiliza o seaborn para plotar a densidade
        sns.kdeplot(intervalos, fill=False)
        plt.title('Densidade dos Intervalos', fontsize=14)
        plt.xlabel('Intervalo (s)', fontsize=12)

    plt.tight_layout()
    plt.savefig('analise.png', dpi=300)
    plt.show()
```
#### 3.1.4 Chamada final das funções
```python
if __name__ == "__main__":
    # leitura de arquivo pcap e plotagem dos gráficos
    pcap_file = "/Users/luizasouza/Git/MC833A/Projeto2/captura.pcap"
    df = leitura_pcap(pcap_file) 
    plotagem_graficos(df)
```
![Resultados extraidos](<dados_extraidos.png>)

### 3.2 Explicação do Código
- **Imports e Intalação:** Setor do código onde são instaladas e importadas as bibliotecas utilizadas no processo. As mais utilizadas são o Scapy para ler os pacotes capturados, Matplotlib para plotagem de gráficos, Pandas e Numpy para manipulação de dados e Seaborn para cálculo da densidade.
- **Extração e Análise de dados:** Para cada pacote transmitido, nessa etapa é realizada a leitura do arquivo Pcap e são extraidos os dados de tamanho do pacote, intervalo de tempo da transmissão e ips de origem e destino. A partir desses dados são encontrados valores posteriormente utilizados para a plotagem de dados.
- **Plotagem de Gráficos:** A partir dos dados encontrados na etapa anterior utilizando o data frame construido para a obtenção dos dados, é feita uma função auxiliar que plota os gráficos buscando uma melhor análise de dados. Alguns dos gráficos tratam de tamanhos, intervalos, throughput e densidade.
- **Chamada de Funções:** Nesse momento são utilizadas todas as funções auxiliares criadas dando como resposta os principais dados coletados e as capturas dos gráficos.

## 4. Resultados

### 4.1 Capturas no Wireshark
- Para setar a topologia de 4 hosts com 1 switch rodamos o comando “sudo mn --topo single,4”. A imagem a seguir mostra o resultado do terminal da máquina virtual após a execução do comando:
![after sudo](<Captura Wireshark/after sudo mn --topo single,4.png>)

- Depois disso abrimos o wireshark no terminal de s1, ainda na máquina virtual para que a captura pudesse ser realizada.
![abertura terminal s1](<Captura Wireshark/Screenshot from 2025-05-04 17-24-49.png>)

- A partir disso, as configurações do wireshark foram definidas (filtro por icmp e seleção das portas a serem ouvidas) e a gravação foi iniciada:
![configuracoes](<Captura Wireshark/configuracoes wireshark.png>)

- O comando “h1 ping -c 200 h3” foi executado, para que os pacotes fossem enviados, enquanto a gravação continuava:
![h1 ping](<Captura Wireshark/depois de h1 ping com wireshark.png>)

- Após isso, foram enviados os pacotes de h2 (h2 ping -c 200 h4), ainda com o wireshark gravando a transmissão:
![h2 ping](<Captura Wireshark/começo h2 ping.png>)

- Por fim, quando todos os pacotes tinham sido enviados, a gravação foi parada e o arquivo foi salvo para que as análises pudessem ser realizadas.
![h2 ping meio](<Captura Wireshark/meio h2 ping.png>)

- Ips de origem e destino:
![h2 ping meio](<Captura Wireshark/depois de h1 ping com wireshark.png>)

- Visão geral Wireshark, com filtro e portas selecionadas:
![h2 ping meio](<Captura Wireshark/configuracoes wireshark.png>)

### 4.2 Métricas de Desempenho
#### 4.2.1 Throughput
![Gráfico de Throughput](<Gráficos/throughput.png>)
- O gráfico de throughout acumulado apresenta a vazão de bits acumuladis ao longo do tempo. Pode-se perceber que o valor é majoritariamente crescente e há apenas um intervalo de tempo no qual a atividade é contante e o valor não aumenta, indicando que naquele intervalo não houve transmissão de bits.

#### 4.2.2 Intervalos entre Pacotes
![Gráfico de Intervalos](<Gráficos/distribuicao_intervalo.png>)
- O histograma de frequência dos intervalos demonstram a frequência dos intervalos de tempo entre pacotes consecutivos. Com ele pode-se perceber que pacotes foram enviados com alta frequência no inicio da tranmissão e que, após um periodo de atividade voltam a ter pacotes enviados mas em menor frequência.

#### 4.2.3 Tamanho dos Pacotes
![Gráfico de Intervalos](<Gráficos/tamanho_pacote.png>)
- O gráfico de tamanho dos pacotes demosntra a variação do tamanho ao longo do tempo de transmissão. Assim como nos casos anteriores, pode-se perceber que houve um período de inatividade mas, desconsiderando esse intervalo, o tamanho dos pacotes enviados permaneceu constante.

#### 4.2.3 Densidade dos Intervalos
![Gráfico de Densidade](<Gráficos/densidade_intervalos.png>)
- O gráfico de densidade dos intervalos demonstra a distribuição dos intervalos em relação ao tempo de transmissão. A densidade representa o intervalo mais comum ente pacotes, e, como pode-se perceber, os dois picos consecutivos são consistentes com os dados observados anteriormente.


## 5. Anexos

### 5.1 Código Fonte
### 5.3 Arquivos de Captura
### 5.3 Gráficos

## 6. Referências
- Slides de aula
- Documentação Pcap
- Guias Matplotlib 
- Storytelling with data