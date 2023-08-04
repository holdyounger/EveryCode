# Self-elevate the script if required
# if (-Not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] 'Administrator')) {
#     if ([int](Get-CimInstance -Class Win32_OperatingSystem | Select-Object -ExpandProperty BuildNumber) -ge 6000) {
#         $Command = "-File `"" + $MyInvocation.MyCommand.Path + "`" " + $MyInvocation.UnboundArguments
#         Start-Process -FilePath PowerShell.exe -Verb RunAs -ArgumentList $Command
#         Exit
#  }
# }

# 获取 InterfaceMetric 为 1 的 -InterfaceIndex
$NetCard = (Get-NetAdapter | Where-Object -FilterScript {$_.ifIndex -eq 10})

Set-NetIPInterface -InterfaceIndex $NetCard.ifIndex -AddressFamily IPv6 -InterfaceMetric 90

Write-Output $NetCard