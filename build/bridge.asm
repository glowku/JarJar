[BITS 16]
[ORG 0x7C00]
start:
    cli                         ; Désactive les interruptions
    ; On saute directement vers ton code à 1Mo (en espérant que ton code gère le passage 32/64b)
    ; Note: En mode réel 16-bit, on ne peut pas sauter à 1Mo facilement.
    ; On va donc juste charger le reste et espérer que QEMU l'a déjà mis là.
    jmp 0x0000:0x7E00           ; Saut de sécurité
    
times 510-($-$$) db 0
dw 0xAA55                       ; Signature de boot magique