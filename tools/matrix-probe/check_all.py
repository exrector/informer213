import re, sys
since = sys.argv[1]
MAP = {33:[11,10,9,8,7,6,5,4,3,2,1,0], 34:list(range(12,-1,-1)), 35:list(range(12,-1,-1)),
       36:[11,10,9,8,7,6,5,4,3,2,1,0], 37:list(range(12,-1,-1)), 38:[11,10,9,8,7,6,5,4,3,2,1,0], 39:[11,10,5,1,0]}
seen=set(); extra=set(); st=set()
for l in open("keys.log"):
    if l[:8] < since: continue
    m=re.search(r"(DOWN|UP) drv=(\d+) sns=(\d+)",l)
    if not m: continue
    d,s=int(m[2]),int(m[3]); (st.add if m[1]=="DOWN" else st.discard)((d,s))
    if m[1]=="DOWN":
        if d in MAP and s in MAP[d]: seen.add((d,s))
        else: extra.add((d,s))
tot=sum(len(v) for v in MAP.values())
print(f"Всего кнопок 81: подключено 80, первая не подключена.\nОтозвались {len(seen)} из 80 подключённых.")
for r,cols in MAP.items():
    miss=[c for c in cols if (r,c) not in seen]
    print(f"строка {r}: {len(cols)-len(miss)}/{len(cols)}" + (f"  НЕТ: столбцы {miss}" if miss else " ✅"))
if extra: print("лишние связи (не по карте):", sorted(extra))
print("сейчас замкнуто:", sorted(st) or "ничего")
