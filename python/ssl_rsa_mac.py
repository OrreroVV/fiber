from cryptography.hazmat.primitives.asymmetric import rsa, padding as rsa_padding
from cryptography.hazmat.primitives import serialization, hashes, padding
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import os, hmac, hashlib

# ---------- 步骤1：服务端生成 RSA 密钥对 ----------
private_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
public_key = private_key.public_key()

# ---------- 步骤2：客户端生成 Pre-Master Secret ----------
pre_master_secret = os.urandom(48)  # 通常为48字节

# ---------- 步骤3：客户端用 RSA 公钥加密 ----------
encrypted_pms = public_key.encrypt(
    pre_master_secret,
    rsa_padding.OAEP(
        mgf=rsa_padding.MGF1(algorithm=hashes.SHA256()),
        algorithm=hashes.SHA256(),
        label=None
    )
)

# ---------- 步骤4：服务端用 RSA 私钥解密 ----------
decrypted_pms = private_key.decrypt(
    encrypted_pms,
    rsa_padding.OAEP(
        mgf=rsa_padding.MGF1(algorithm=hashes.SHA256()),
        algorithm=hashes.SHA256(),
        label=None
    )
)
assert decrypted_pms == pre_master_secret

# ---------- 步骤5：派生密钥（加密密钥 + MAC密钥） ----------
master_secret = hashlib.sha256(pre_master_secret).digest()
enc_key = master_secret[:16]   # AES-128
mac_key = master_secret[16:]   # HMAC key（也16字节）

print(master_secret[:16], "-", master_secret[16:])


# ---------- 客户端准备消息并加密 ----------
plaintext = b"RSA handshake + MAC protect this TLS message."

# Step 1: MAC（MAC-then-encrypt）
mac = hmac.new(mac_key, plaintext, hashlib.sha256).digest()
message_with_mac = plaintext + mac

# Step 2: AES-CBC 加密
iv = os.urandom(16)
padder = padding.PKCS7(128).padder()
padded_data = padder.update(message_with_mac) + padder.finalize()

cipher = Cipher(algorithms.AES(enc_key), modes.CBC(iv))
encryptor = cipher.encryptor()
ciphertext = encryptor.update(padded_data) + encryptor.finalize()

print("[Client] 已发送 RSA 加密的 Pre-Master Secret、IV 和加密消息")

# ---------- 服务端接收并解密 ----------
cipher = Cipher(algorithms.AES(enc_key), modes.CBC(iv))
decryptor = cipher.decryptor()
decrypted_padded = decryptor.update(ciphertext) + decryptor.finalize()

# 去填充
unpadder = padding.PKCS7(128).unpadder()
decrypted_with_mac = unpadder.update(decrypted_padded) + unpadder.finalize()

# 拆出明文和MAC
recv_plaintext = decrypted_with_mac[:-32]
recv_mac = decrypted_with_mac[-32:]

# 验证 HMAC
expected_mac = hmac.new(mac_key, recv_plaintext, hashlib.sha256).digest()
valid = hmac.compare_digest(recv_mac, expected_mac)

# 输出
print("\n[Server] 解密消息:", recv_plaintext.decode())
print("[Server] MAC 是否一致:", valid)
