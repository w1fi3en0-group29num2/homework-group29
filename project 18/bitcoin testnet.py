import requests

# 设置Blockcypher API的相关参数
api_token = '497a821fc8a746068a9db293a329c1d8'
api_base_url = 'https://api.blockcypher.com/v1/btc/test3/'

# 创建交易数据
transaction_data = {
    "inputs": [{
        "addresses": ["dingqiao"]
    }],
    "outputs": [{
        "addresses": ["dingqiao"],
        "value": 666
    }]
}

# 发送交易请求
response = requests.post(api_base_url + 'txs/new', json=transaction_data, params={'token': api_token})
response_json = response.json()

# 获取交易哈希
tx_hash = response_json['tx']['hash']

# 解析交易数据
tx_data = response_json['tx']

print(tx_data)
