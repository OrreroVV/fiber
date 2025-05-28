import os
import hmac
import hashlib
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives.asymmetric import dh
from cryptography.hazmat.primitives import padding

# 1. DH 密钥交换
parameters = dh.generate_parameters(generator=2, key_size=2048)
client_private = parameters.generate_private_key()
server_private = parameters.generate_private_key()
client_public = client_private.public_key()
server_public = server_private.public_key()
shared_secret = client_private.exchange(server_public)
assert shared_secret == server_private.exchange(client_public)

# 2. 派生主密钥 -> 加密密钥 & MAC 密钥（简单示例用 SHA256 hash 拆分）
master_secret = hashlib.sha256(shared_secret).digest()
encryption_key = master_secret[:16]  # AES-128
mac_key = master_secret[16:]        # 余下作为 HMAC key

# 3. 客户端准备发送消息
plaintext = b"This is a confidential TLS message."

# 3.1 计算 MAC（先计算 MAC，再加密）
mac = hmac.new(mac_key, plaintext, hashlib.sha256).digest()

# 3.2 拼接消息 + MAC
message_with_mac = plaintext + mac

# 3.3 AES-CBC 加密
iv = os.urandom(16)
padder = padding.PKCS7(128).padder()
padded_data = padder.update(message_with_mac) + padder.finalize()
cipher = Cipher(algorithms.AES(encryption_key), modes.CBC(iv))
encryptor = cipher.encryptor()
ciphertext = encryptor.update(padded_data) + encryptor.finalize()

print("[Client] 发送加密消息和 IV")

# ------------------------ 服务端接收并处理 ------------------------

# 4. 服务端接收 ciphertext 和 IV，解密
cipher = Cipher(algorithms.AES(encryption_key), modes.CBC(iv))
decryptor = cipher.decryptor()
decrypted_padded = decryptor.update(ciphertext) + decryptor.finalize()

# 4.1 去填充
unpadder = padding.PKCS7(128).unpadder()
decrypted = unpadder.update(decrypted_padded) + unpadder.finalize()

# 4.2 拆出原文与 MAC
recv_plaintext = decrypted[:-32]
recv_mac = decrypted[-32:]

# 4.3 重新计算 MAC 验证完整性
expected_mac = hmac.new(mac_key, recv_plaintext, hashlib.sha256).digest()
is_valid = hmac.compare_digest(recv_mac, expected_mac)

# 输出结果
print("\n[Server] 解密后明文:", recv_plaintext.decode())
print("[Server] 校验 MAC 是否一致:", is_valid)
