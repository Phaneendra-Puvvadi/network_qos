#!/usr/bin/env python3
from flask import Flask, render_template, jsonify, send_file
import json
import os
from datetime import datetime
import io
from reportlab.lib.pagesizes import letter
from reportlab.lib.styles import getSampleStyleSheet
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle
from reportlab.lib import colors

app = Flask(__name__)

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
    try:
        with open('stats.json', 'r') as f:
            data = json.load(f)
        return jsonify(data)
    except:
        return jsonify({
            'timestamp': 0,
            'congestion_score': 0,
            'congestion_predicted': False,
            'queues': {'high': 0, 'medium': 0, 'low': 0},
            'packets_per_sec': 0,
            'bytes_per_sec': 0,
            'total_packets': 0,
            'total_bytes': 0
        })

@app.route('/download-report')
def download_report():
    buffer = io.BytesIO()
    doc = SimpleDocTemplate(buffer, pagesize=letter)
    styles = getSampleStyleSheet()
    story = []
    
    # Title
    title = Paragraph("<b>Network QoS Router - Simulation Report</b>", styles['Title'])
    story.append(title)
    story.append(Spacer(1, 20))
    
    # Timestamp
    timestamp = Paragraph(f"<b>Report Generated:</b> {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}", styles['Normal'])
    story.append(timestamp)
    story.append(Spacer(1, 20))
    
    # Get current stats
    try:
        with open('stats.json', 'r') as f:
            data = json.load(f)
    except:
        data = {
            'congestion_score': 0,
            'congestion_predicted': False,
            'queues': {'high': 0, 'medium': 0, 'low': 0},
            'total_packets': 0,
            'total_bytes': 0
        }
    
    # Statistics Table
    stats_data = [
        ['Metric', 'Value'],
        ['Total Packets Processed', str(data.get('total_packets', 0))],
        ['Total Bytes Processed', f"{data.get('total_bytes', 0):,} bytes"],
        ['Congestion Score', f"{data.get('congestion_score', 0):.3f}"],
        ['Congestion Status', 'CONGESTED' if data.get('congestion_predicted', False) else 'NORMAL'],
        ['High Priority Queue', str(data.get('queues', {}).get('high', 0))],
        ['Medium Priority Queue', str(data.get('queues', {}).get('medium', 0))],
        ['Low Priority Queue', str(data.get('queues', {}).get('low', 0))],
    ]
    
    stats_table = Table(stats_data)
    stats_table.setStyle(TableStyle([
        ('BACKGROUND', (0, 0), (-1, 0), colors.grey),
        ('TEXTCOLOR', (0, 0), (-1, 0), colors.whitesmoke),
        ('ALIGN', (0, 0), (-1, -1), 'CENTER'),
        ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
        ('FONTSIZE', (0, 0), (-1, 0), 14),
        ('BOTTOMPADDING', (0, 0), (-1, 0), 12),
        ('BACKGROUND', (0, 1), (-1, -1), colors.beige),
        ('GRID', (0, 0), (-1, -1), 1, colors.black)
    ]))
    
    story.append(stats_table)
    story.append(Spacer(1, 20))
    
    # Project Info
    info = Paragraph("<b>Project:</b> Adaptive Network Traffic Prioritization Using Real-Time Packet Behavior Analysis", styles['Normal'])
    story.append(info)
    story.append(Spacer(1, 10))
    
    description = Paragraph("This system uses predictive congestion detection and adaptive priority scheduling to ensure Quality of Service for time-sensitive network traffic.", styles['Normal'])
    story.append(description)
    
    doc.build(story)
    buffer.seek(0)
    
    return send_file(
        buffer,
        as_attachment=True,
        download_name=f'network_qos_report_{datetime.now().strftime("%Y%m%d_%H%M%S")}.pdf',
        mimetype='application/pdf'
    )

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
