import { ComponentFixture, TestBed } from '@angular/core/testing';

import { EditConcentradorComponent } from './edit-concentrador.component';

describe('EditConcentradorComponent', () => {
  let component: EditConcentradorComponent;
  let fixture: ComponentFixture<EditConcentradorComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ EditConcentradorComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(EditConcentradorComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
