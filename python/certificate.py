from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives import serialization

# -------- 生成 RSA 密钥对 --------
private_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
public_key = private_key.public_key()

# -------- 签名 --------
message = b"Important message from server"
hasher = hashes.Hash(hashes.SHA256())
hasher.update(message)
digest = hasher.finalize()

signature = private_key.sign(
    digest,
    padding.PKCS1v15(),
    hashes.SHA256()
)

print("[Sender] 已生成签名")

# -------- 验证签名 --------
try:
    # 重新计算哈希
    hasher = hashes.Hash(hashes.SHA256())
    hasher.update(message)
    digest_check = hasher.finalize()

    public_key.verify(
        signature,
        digest_check,
        padding.PKCS1v15(),
        hashes.SHA256()
    )
    print("[Receiver] 签名验证成功 ✅")
except Exception as e:
    print("[Receiver] 签名验证失败 ❌", e)
