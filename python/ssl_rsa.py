import os
import hashlib
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC

# 1. 服务端生成RSA密钥对
# --- Step 1: Server generates RSA key pair ---
private_key = rsa.generate_private_key(
    public_exponent=65537,
    key_size=2048
)
public_key = private_key.public_key()

# 2. 客户端生成 Pre-Master Secret (32 字节随机数)
# --- Step 2: Client generates Pre-Master Secret ---
pre_master_secret = os.urandom(32)
print("客户端生成的 Pre-Master Secret:", pre_master_secret.hex())

# 3. 客户端使用服务端公钥加密 Pre-Master Secret
# --- Step 3: Client encrypts Pre-Master Secret with Server's public key ---
ciphertext = public_key.encrypt(
    pre_master_secret,
    padding.OAEP(
        mgf=padding.MGF1(algorithm=hashes.SHA256()),
        algorithm=hashes.SHA256(),
        label=None
    )
)
print("\n客户端加密后的 Pre-Master Secret:", ciphertext.hex())

# 4. 服务端用私钥解密 Pre-Master Secret
# --- Step 4: Server decrypts Pre-Master Secret with its private key ---
decrypted_pre_master_secret = private_key.decrypt(
    ciphertext,
    padding.OAEP(
        mgf=padding.MGF1(algorithm=hashes.SHA256()),
        algorithm=hashes.SHA256(),
        label=None
    )
)
print("\n服务端解密后的 Pre-Master Secret:", decrypted_pre_master_secret.hex())

# 5. 生成 ClientRandom 和 ServerRandom（模拟客户端和服务端的随机数）
# --- Step 5: Client and Server generate random values ---
client_random = os.urandom(32)
server_random = os.urandom(32)
print("\n客户端随机数 (ClientRandom):", client_random.hex())
print("服务端随机数 (ServerRandom):", server_random.hex())

# 6. 计算 Master Secret (使用 PRF，这里我们使用 HMAC + SHA-256)
# --- Step 6: Both Client and Server compute Master Secret ---
def prf(pre_master_secret, label, data):
    """伪随机函数 (PRF) 计算 Master Secret"""
    hmac = hashlib.new('sha256', pre_master_secret + label.encode() + data)
    return hmac.digest()

# 计算 Master Secret
label = "master secret"
data = client_random + server_random
master_secret = prf(decrypted_pre_master_secret, label, data)
print("\n计算出的 Master Secret:", master_secret.hex())

# 7. 计算会话密钥（这里只是简单模拟，用 Master Secret 派生其他密钥）
# --- Step 7: Derive session keys (encryption and MAC keys) from Master Secret ---
def derive_key(master_secret, label, data):
    """通过 Master Secret 派生会话密钥"""
    return prf(master_secret, label, data)

# 派生加密密钥和 MAC 密钥（模拟）
encryption_key = derive_key(master_secret, "key expansion", b"encryption")
mac_key = derive_key(master_secret, "key expansion", b"mac")
print("\n派生的加密密钥:", encryption_key.hex())
print("派生的 MAC 密钥:", mac_key.hex())

# ------------------------------- 结束 TLS 握手过程 -------------------------------
