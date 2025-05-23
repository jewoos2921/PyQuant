subdir('LowLatencyTrading')
executable('quant',
           'main.cpp',
           LowLatencyTrading_common_src,
           LowLatencyTrading_exchange_src,
           LowLatencyTrading_trading_src,
           include_directories : [
               LowLatencyTrading_common_inc,
               LowLatencyTrading_exchange_inc,
               LowLatencyTrading_trading_inc,
               LowLatencyTrading_inc
           ],
           install : true
)
