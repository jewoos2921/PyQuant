LowLatencyTrading_trading_src = files('trading_main.cpp')
LowLatencyTrading_trading_inc = include_directories('.')

# 하위 디렉토리 추가
subdir('market_data')
# subdir('order_gate')
# subdir('strategy')

# 소스 파일 합치기
LowLatencyTrading_trading_src += market_data_consumer_src
# LowLatencyTrading_trading_src += matcher_src
# LowLatencyTrading_trading_src += order_server_src