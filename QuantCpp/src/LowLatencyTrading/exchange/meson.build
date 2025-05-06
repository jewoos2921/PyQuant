LowLatencyTrading_exchange_src = files('exchange_main.cpp')
LowLatencyTrading_exchange_inc = include_directories('.')

# 하위 디렉토리 추가
subdir('market_data')
subdir('matcher')
subdir('old_server')

# 소스 파일 합치기
LowLatencyTrading_exchange_src += market_data_src
LowLatencyTrading_exchange_src += matcher_src 
LowLatencyTrading_exchange_src += old_server_src