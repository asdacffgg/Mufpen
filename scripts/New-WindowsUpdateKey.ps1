param([string]$PublicKey = "$PSScriptRoot/../resources/update-public-key.txt")

# The private key remains in the current user's Windows CNG key store. Export
# directly into the Actions secret's stdin; never write it into the repository.
$ErrorActionPreference = 'Stop'
$keyName = 'Mufpen.WindowsUpdate.ReleaseSigning'
# The fork's existing signing key was created before the repo-wide rename.
# Keep reading the legacy container so the committed public key stays valid.
$legacyKeyName = 'Muffin.WindowsUpdate.ReleaseSigning'
if ([Security.Cryptography.CngKey]::Exists($keyName)) {
    # Reuse the same key after an interrupted secret upload; never rotate trust.
    $key = [Security.Cryptography.CngKey]::Open($keyName)
} elseif ([Security.Cryptography.CngKey]::Exists($legacyKeyName)) {
    $key = [Security.Cryptography.CngKey]::Open($legacyKeyName)
} else {
    if (Test-Path -LiteralPath $PublicKey) { throw 'The existing public key has no matching local private key. Do not rotate update trust.' }
    $parameters = [Security.Cryptography.CngKeyCreationParameters]::new()
    $parameters.ExportPolicy = [Security.Cryptography.CngExportPolicies]::AllowExport -bor [Security.Cryptography.CngExportPolicies]::AllowPlaintextExport
    $key = [Security.Cryptography.CngKey]::Create([Security.Cryptography.CngAlgorithm]::ECDsaP256, $keyName, $parameters)
}
try {
    $public = ([BitConverter]::ToString($key.Export([Security.Cryptography.CngKeyBlobFormat]::EccPublicBlob))).Replace('-', '').ToLowerInvariant()
    if ((Test-Path -LiteralPath $PublicKey) -and ([IO.File]::ReadAllText([IO.Path]::GetFullPath($PublicKey)).Trim() -cne $public)) {
        throw 'The local signing key does not match the existing public key. Do not rotate update trust.'
    }
    $secret = [Convert]::ToBase64String($key.Export([Security.Cryptography.CngKeyBlobFormat]::EccPrivateBlob))
    $secret | & gh secret set MUFPEN_UPDATE_SIGNING_KEY --repo asdacffgg/Mufpen
    if ($LASTEXITCODE -ne 0) { throw 'Could not provision the GitHub Actions signing secret.' }
    if (-not (Test-Path -LiteralPath $PublicKey)) {
        [IO.File]::WriteAllText([IO.Path]::GetFullPath($PublicKey), $public + "`n", [Text.UTF8Encoding]::new($false))
    }
    Write-Output 'Public key saved; private key retained in Windows CNG and GitHub Actions Secrets.'
} finally {
    $secret = $null
    $key.Dispose()
}
