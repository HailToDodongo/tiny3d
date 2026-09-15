#!/usr/bin/env python3
"""Inline depth-bench dumps into tools/depth_report.html so the page is self-contained.

    tools/depth_report_embed.py out.html examples/98_depthtest/data/*.json [--fragment]

--fragment strips the <!doctype>/<html>/<head>/<body> wrapper (for hosts that wrap the page themselves).
"""
import sys, json, re, os
args = [a for a in sys.argv[1:] if not a.startswith("--")]
fragment = "--fragment" in sys.argv
out, files = args[0], args[1:]
here = os.path.dirname(os.path.abspath(__file__))
html = open(os.path.join(here, "depth_report.html"), encoding="utf-8").read()
dumps = [json.load(open(f, encoding="utf-8")) for f in files]
for d, f in zip(dumps, files):
    d["file"] = os.path.basename(f)
blob = json.dumps(dumps, separators=(",", ":")).replace("</", "<\\/")
inject = "<script>window.DT_EMBEDDED = " + blob + ";</script>\n<script>"
html = html.replace("<script>\n(() => {", inject + "\n(() => {", 1)
if fragment:
    m = re.search(r"<head>(.*?)</head>\s*<body>(.*)</body>", html, re.S)
    head, body = m.group(1), m.group(2)
    head = re.sub(r"<meta[^>]*>\s*", "", head)  # host supplies charset/viewport
    html = head.strip() + "\n" + body.strip() + "\n"
open(out, "w", encoding="utf-8").write(html)
print(f"wrote {out}: {len(html)/1e6:.1f} MB, {len(dumps)} dumps")
