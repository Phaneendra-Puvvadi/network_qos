from flask import Flask, render_template, jsonify, send_file
import json
import os
import sys

# Add web directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'web'))

app = Flask(__name__, 
            template_folder='../web/templates',
            static_folder='../web/static')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/learn')
def learn():
    return render_template('learn.html')

@app.route('/team')
def team():
    return render_template('team.html')

@app.route('/help')
def help_page():
    return render_template('help.html')

@app.route('/api/stats')
def get_stats():
    return jsonify({
        'timestamp': 0,
        'congestion_score': 0.45,
        'congestion_predicted': False,
        'queues': {'high': 0, 'medium': 0, 'low': 0},
        'packets_per_sec': 0,
        'bytes_per_sec': 0,
        'total_packets': 0,
        'total_bytes': 0
    })

@app.route('/download-report')
def download_report():
    from io import BytesIO
    from datetime import datetime
    from reportlab.lib.pagesizes import letter
    from reportlab.lib.styles import getSampleStyleSheet
    from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer
    
    buffer = BytesIO()
    doc = SimpleDocTemplate(buffer, pagesize=letter)
    styles = getSampleStyleSheet()
    story = []
    
    title = Paragraph("<b>Network QoS Router - Demo Report</b>", styles['Title'])
    story.append(title)
    story.append(Spacer(1, 20))
    
    timestamp = Paragraph(f"<b>Generated:</b> {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}", styles['Normal'])
    story.append(timestamp)
    story.append(Spacer(1, 20))
    
    info = Paragraph("<b>Project:</b> Adaptive Network Traffic Prioritization", styles['Normal'])
    story.append(info)
    
    doc.build(story)
    buffer.seek(0)
    
    return send_file(
        buffer,
        as_attachment=True,
        download_name=f'network_qos_demo_{datetime.now().strftime("%Y%m%d")}.pdf',
        mimetype='application/pdf'
    )

# Vercel serverless function handler
app = app
