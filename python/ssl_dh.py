import os
import hashlib
from cryptography.hazmat.primitives.asymmetric import dh
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC

# 1. 创建 Diffie-Hellman 参数（这是一个共享的基础，客户端和服务端都使用这个参数）
# --- Step 1: Generate Diffie-Hellman parameters ---
parameters = dh.generate_parameters(generator=2, key_size=2048)

# 2. 客户端生成 Diffie-Hellman 密钥对
# --- Step 2: Client generates DH key pair ---
client_private_key = parameters.generate_private_key()
client_public_key = client_private_key.public_key()

# 3. 服务端生成 Diffie-Hellman 密钥对
# --- Step 3: Server generates DH key pair ---
server_private_key = parameters.generate_private_key()
server_public_key = server_private_key.public_key()

# 4. 客户端和服务端交换公钥并计算共享密钥
# --- Step 4: Client computes shared secret with server's public key ---
client_shared_secret = client_private_key.exchange(server_public_key)

# --- Step 5: Server computes shared secret with client's public key ---
server_shared_secret = server_private_key.exchange(client_public_key)

# 5. 确认双方共享密钥一致
# --- Step 6: Ensure the shared secret is the same on both sides ---
assert client_shared_secret == server_shared_secret, "共享密钥不匹配！"

print("\n客户端计算的共享密钥:", client_shared_secret.hex())
print("服务端计算的共享密钥:", server_shared_secret.hex())

# 6. 生成 Master Secret（使用共享密钥并加盐）
# --- Step 7: Generate Master Secret from shared secret ---
label = "master secret"
data = os.urandom(32)  # 随机数据作为扩展，通常是客户端和服务端的随机数

# 使用共享密钥和一些随机数据生成 Master Secret
def prf(shared_secret, label, data):
    """伪随机函数 (PRF) 计算 Master Secret"""
    hmac = hashlib.new('sha256', shared_secret + label.encode() + data)
    return hmac.digest()

master_secret = prf(client_shared_secret, label, data)
print("\n生成的 Master Secret:", master_secret.hex())

# 7. 派生会话密钥（模拟生成加密密钥和 MAC 密钥）
# --- Step 8: Derive session keys from Master Secret ---
def derive_key(master_secret, label, data):
    """通过 Master Secret 派生会话密钥"""
    return prf(master_secret, label, data)

# 派生加密密钥和 MAC 密钥（模拟）
encryption_key = derive_key(master_secret, "key expansion", b"encryption")
mac_key = derive_key(master_secret, "key expansion", b"mac")
print("\n派生的加密密钥:", encryption_key.hex())
print("派生的 MAC 密钥:", mac_key.hex())

# ------------------------------- 结束 TLS 握手过程 -------------------------------
