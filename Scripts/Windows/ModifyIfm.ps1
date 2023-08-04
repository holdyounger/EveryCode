# Self-elevate the script if required
# if (-Not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] 'Administrator')) {
#     if ([int](Get-CimInstance -Class Win32_OperatingSystem | Select-Object -ExpandProperty BuildNumber) -ge 6000) {
#         $Command = "-File `"" + $MyInvocation.MyCommand.Path + "`" " + $MyInvocation.UnboundArguments
#         Start-Process -FilePath PowerShell.exe -Verb RunAs -ArgumentList $Command
#         Exit
#  }
# }

$ChangeIfm = 1
$WangToChangeIfm = 90

# 获取 InterfaceMetric 为 1 的 -InterfaceIndex
$NetCard = (Get-NetIPInterface -AddressFamily IPv4 -InterfaceMetric $ChangeIfm).ifAlias
$netInterfaceIndex = (Get-NetIPInterface -AddressFamily IPv4 -InterfaceMetric $ChangeIfm).ifIndex

Write-Output "检索InterfaceMetric为 [${ChangeIfm}] 列表: `n ${NetCard}"
Write-Output "Index 为:  ${netInterfaceIndex}"

Set-NetIPInterface -AddressFamily IPv4 -InterfaceIndex $netInterfaceIndex -InterfaceMetric $WangToChangeIfm

Write-Output "已修改 ${NetCard} 的 InterfaceMetric 为 ${WangToChangeIfm}"