# subdir('LowLatencyTrading')

# executable('quant',
#     'main.cpp',
#     # 필요하다면 LowLatencyTrading에서 정의한 라이브러리 추가
#     # link_with: liblowlatencytrading,
#     LowLatencyTrading_common_src,
#   LowLatencyTrading_exchange_src,
#      include_directories: [
#     LowLatencyTrading_common_inc,
#     LowLatencyTrading_exchange_inc
#   ],
#     install: true
# )
subdir('LowLatencyTrading')

executable('quant',
           'main.cpp',
           # 필요하다면 LowLatencyTrading에서 정의한 라이브러리 추가
           # link_with: liblowlatencytrading,
           LowLatencyTrading_common_src,
           LowLatencyTrading_exchange_src,
           include_directories : [
               LowLatencyTrading_common_inc,
               LowLatencyTrading_exchange_inc,
               LowLatencyTrading_inc
           ],
           install : true
)
