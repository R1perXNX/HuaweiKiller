# Huawei EDR Killer – Proof of Concept  

⚠️ **Disclaimer:**  
This project is intended **for educational and research purposes only**.

---
**CVE-2023-52972 Exploit**

This proof of concept demonstrates the exploitation of a vulnerability in the Huawei Audio Driver **`HWAuidoOs2Ec.sys`** (CVE-2023-52972).  
The vulnerability allows low-privileged users to **bypass SDDL permission checks**. A successful exploit of this vulnerability can lead to EDR termination. In this example, the exploit is used to **terminate Windows Defender**.

My team and I discovered this vulnerability in January 2025 using symbolic execution with the Angr framework. Before that date, no publicly available details about the CVE existed, so we reported it to Huawei. After our disclosure, Huawei released additional details regarding the CVE-2023-52972.<br><br><br>
<img width="720" height="480" alt="image" src="https://github.com/user-attachments/assets/473595dc-e882-4c00-be47-695cd55803ee" /><br><br><br>
Although an official patch exists, this driver can still be leveraged in a BYOVD  attack, as it is not blocked by the Windows Driver Blacklist at the time of writing (Tested on both Windows 10 and Windows 11).

## Credits
Thanks to @mnznndr97 and @enkomio for giving me the opportunity to work with them.
 




